#define GAUSSIAN_RADIUS 7

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);


static float Gaussian[15] =
{
    0.00006103515625f,
    0.0008544921875f,
    0.00555419921875f,
    0.022216796875f,
    0.06109619140625f,
    0.1221923828125f,
    0.18328857421875f,
    0.20947265625f,
    0.18328857421875f,
    0.1221923828125f,
    0.06109619140625f,
    0.022216796875f,
    0.00555419921875f,
    0.0008544921875f,
    0.00006103515625f
};

cbuffer BlurParams : register(b0)
{
    // = float coefficients[GAUSSIAN_RADIUS + 1]
    // radius <= GAUSSIAN_RADIUS, direction 0 = horizontal, 1 = vertical
    int2 radiusAndDirection;
    uint2 iWinSize;
    
    float fBlurPower;
    uint3 Padding;
}

[numthreads(8, 8, 1)]
void Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= iWinSize.x || pixel.y >= iWinSize.y)
        return;
    
    int radius = radiusAndDirection.x;
    int2 dir = int2(1 - radiusAndDirection.y, radiusAndDirection.y);

    float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);
        
        //float Gaussian[15] = { 0.000067f, 0.000437f, 0.002216f, 0.008765f, 0.027001f, 0.064764f, 0.121012f, 0.199500f, 0.121012f, 0.064764f, 0.027001f, 0.008765f, 0.002216f, 0.000437f, 0.000067f };
    for (int i = -radius; i <= radius; ++i)
    {
        uint cIndex = (uint) (i + radius);
            //accumulatedValue += coefficients[cIndex >> 2][cIndex & 3] * inputTexture[mad(i, dir, pixel)];
        int2 iIndex = mad(i, dir, pixel);
        int3 i3Index = int3(iIndex, 0);
        accumulatedValue += inputTexture.Load(i3Index) * Gaussian[cIndex] * fBlurPower;
    }
        
    outputTexture[pixel] = accumulatedValue;
}

cbuffer RadialParams : register(b1)
{
    float2 g_vCenterTexPos;
    float g_fRadialBlur_Power;
    uint g_isTex;
    
    vector g_vCenterPos;
}

static float fSize[15] = { -0.07f, -0.06f, -0.05f, -0.04f, -0.03f, -0.02f, -0.01f, 0.f, 0.01f, 0.02f, 0.03f, 0.04f, 0.05f, 0.06f, 0.07f };

[numthreads(8,8,1)]
void Radial_Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= 1280 || pixel.y >= 720)
        return;
    
    float2 UV;
    UV.x = pixel.x / (float) 1280;
    UV.y = pixel.y / (float) 720;
    
    float2 Dir;
    if (1 == g_isTex)
    {
        Dir = g_vCenterTexPos - UV;
    }
    else
    {
        float2 vPos = g_vCenterPos.xy;
        
        vPos.x = (vPos.x + 1.f) * 0.5f;
        vPos.y = (vPos.y - 1.f) * -0.5f;
        
        Dir = vPos - UV;
    }
    
    float2 DirMulti = Dir * g_fRadialBlur_Power;
    
    float2 vTex;
    vector vColor = 0.f;
    for (uint i = 0; i < 15; ++i)
    {
        vTex = (DirMulti * fSize[i]) + UV;
        vTex.x = vTex.x * (float) 1280;
        vTex.y = vTex.y * (float) 720;
        
        int3 iIndex;
        iIndex.x = (int) vTex.x;
        iIndex.y = (int) vTex.y;
        iIndex.z = 0;
        vColor += inputTexture.Load(iIndex) * Gaussian[i];
    }

    outputTexture[pixel] = vColor;

}

Texture2D<float4> Depth_Velocity_Texture : register(t1);


[numthreads(8, 8, 1)]
void Motion_Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= 1280 || pixel.y >= 720)
        return;
    
    uint iNumBlurSample = 10;
    
    int3 iCurIndex = int3(pixel, 0);
    vector vDepth_Velocity_Desc = Depth_Velocity_Texture.Load(iCurIndex);
    
    float2 Velocity = vDepth_Velocity_Desc.zw / (float) iNumBlurSample;
    
    float Depth = vDepth_Velocity_Desc.x;
    
    float2 vTex;
    vTex.x = pixel.x / (float) 1280;
    vTex.y = pixel.y / (float) 720;
    
    uint iCnt = 1;
    
    vector BColor = 0.f;
    float BDepth = 0.f;
    
    vector vColor = inputTexture.Load(iCurIndex);
    
    for (uint i = iCnt; i < iNumBlurSample; ++i)
    {
        float2 UV = vTex + Velocity * (float) i;
        UV.x = UV.x * (float) 1280;
        UV.y = UV.y * (float) 720;
        
        int3 iIndex;
        iIndex.x = (int) UV.x;
        iIndex.y = (int) UV.y;
        iIndex.z = 0;
        BColor = inputTexture.Load(iIndex);
        BDepth = Depth_Velocity_Texture.Load(iIndex).x;
        
        if(Depth < BDepth + 0.04f)
        {
            iCnt++;
            vColor += BColor;
        }
    }

    vColor /= (float) iCnt;
    
    outputTexture[pixel] = vColor;
}

SamplerState LinearSampler : register(s0);


float RGB2Lum(float3 RGB)
{
    return sqrt(dot(RGB, float3(0.299f, 0.587f, 0.114f)));
}

[numthreads(8, 8, 1)]
void FXAA(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x >= 1280 || pixel.y >= 720)
        return;
    
    float2 vUV;
    vUV.x = pixel.x / 1280.f;
    vUV.y = pixel.y / 720.f;
    
    float3 RGB_M = inputTexture.Load(int3(pixel, 0)).rgb;
    
    float3 RGB_L = inputTexture.Load(int3(pixel + int2(-1, 0), 0)).rgb;
    float3 RGB_R = inputTexture.Load(int3(pixel + int2(1, 0), 0)).rgb;
    float3 RGB_U = inputTexture.Load(int3(pixel + int2(0, -1), 0)).rgb;
    float3 RGB_D = inputTexture.Load(int3(pixel + int2(0, 1), 0)).rgb;
    
    float Lum_M = RGB2Lum(RGB_M);
    
    float Lum_L = RGB2Lum(RGB_L);
    float Lum_R = RGB2Lum(RGB_R);
    float Lum_U = RGB2Lum(RGB_U);
    float Lum_D = RGB2Lum(RGB_D);
    
    float LumMin = min(Lum_M, min(min(Lum_D, Lum_U), min(Lum_L, Lum_R)));
    float LumMax = max(Lum_M, max(max(Lum_D, Lum_U), max(Lum_L, Lum_R)));
    
    float Lum_Range = LumMax - LumMin;
    
    vector vColor = 1.f;
    //if (Lum_Range < max(0.0312, LumMax * 0.125))
    if (Lum_Range < 0.f)
        {
        vColor = vector(RGB_M, 1.f);
        outputTexture[pixel] = vColor;
        return;
    }
    
    float3 RGB_DL = inputTexture.Load(int3(pixel + int2(-1, 1), 0)).rgb;
    float3 RGB_DR = inputTexture.Load(int3(pixel + int2(+1, 1), 0)).rgb;
    float3 RGB_UL = inputTexture.Load(int3(pixel + int2(-1, -1), 0)).rgb;
    float3 RGB_UR = inputTexture.Load(int3(pixel + int2(1, 1), 0)).rgb;
    
    float Lum_DL = RGB2Lum(RGB_DL);
    float Lum_DR = RGB2Lum(RGB_DR);
    float Lum_UL = RGB2Lum(RGB_UL);
    float Lum_UR = RGB2Lum(RGB_UR);
    
    float Lum_UD = Lum_D + Lum_U;
    float Lum_LR = Lum_L + Lum_R;
    
    float Lum_LC = Lum_DL + Lum_UL;
    float Lum_DC = Lum_DL + Lum_DR;
    float Lum_RC = Lum_DR + Lum_UR;
    float Lum_UC = Lum_UR + Lum_UL;
    
    float Edge_Horizontal = abs(-2.f * Lum_L + Lum_LC) + abs(-2.f * Lum_M + Lum_UD) * 2.f + abs(-2.f * Lum_R + Lum_RC);
    float Edge_Vertical = abs(-2.f * Lum_U + Lum_UC) + abs(-2.f * Lum_M + Lum_LR) * 2.f + abs(-2.f * Lum_D + Lum_DC);
    
    bool isHorizontal = (Edge_Horizontal >= Edge_Vertical);
    
    float Lum1 = isHorizontal ? Lum_D : Lum_L;
    float Lum2 = isHorizontal ? Lum_U : Lum_R;
    
    float Gradient1 = Lum1 - Lum_M;
    float Gradient2 = Lum2 - Lum_M;
    
    bool is1Steepest = abs(Gradient1) >= abs(Gradient2);
    
    float GradientScaled = 0.25f * max(abs(Gradient1), abs(Gradient2));

    float stepLegth = isHorizontal ? 1.f / 720.f : 1.f / 1280.f;
    
    float Lum_LA = 0.f;
    
    if (is1Steepest)
    {
        stepLegth = -stepLegth;
        Lum_LA = 0.5f * (Lum1 + Lum_M);
    }
    else
    {
        Lum_LA = 0.5f * (Lum2 + Lum_M);
    }
    
    float2 CurrentUV = vUV;
    if (isHorizontal)
    {
        CurrentUV.y += stepLegth * 0.5f;
    }
    else
    {
        CurrentUV.x += stepLegth * 0.5f;
    }
    
    float2 offset = isHorizontal ? float2(1.f / 1280.f, 0.f) : float2(0.f, 1.f / 720.f);
    
    float2 UV1 = CurrentUV - offset;
    float2 UV2 = CurrentUV + offset;
    
    int2 iTex;
    iTex.x = UV1.x * 1280.f;
    iTex.y = UV1.y * 720.f;
    float Lum_End1 = RGB2Lum(inputTexture.Load(int3(iTex, 0)).rgb);
    
    iTex.x = UV2.x * 1280.f;
    iTex.y = UV2.y * 720.f;
    float Lum_End2 = RGB2Lum(inputTexture.Load(int3(iTex, 0)).rgb);
    
    Lum_End1 -= Lum_LA;
    Lum_End2 -= Lum_LA;
    
    bool Reached1 = abs(Lum_End1) >= GradientScaled;
    bool Reached2 = abs(Lum_End2) >= GradientScaled;
    bool ReachBoth = Reached1 && Reached2;
    
    if (false == Reached1)
    {
        UV1 -= offset;
    }
    
    if (false == Reached2)
    {
        UV2 += offset;
    }
    
    if (false == ReachBoth)
    {
        for (uint i = 2; i < 12; ++i)
        {
            if (false == Reached1)
            {
                iTex.x = UV1.x * 1280.f;
                iTex.y = UV1.y * 720.f;
                Lum_End1 = RGB2Lum(inputTexture.Load(int3(iTex, 0)).rgb);
                Lum_End1 = Lum_End1 - Lum_LA;
            }
            
            if (false == Reached2)
            {
                iTex.x = UV2.x * 1280.f;
                iTex.y = UV2.y * 720.f;
                Lum_End2 = RGB2Lum(inputTexture.Load(int3(iTex, 0)).rgb);
                Lum_End2 = Lum_End2 - Lum_LA;
            }
            
            Reached1 = abs(Lum_End1) >= GradientScaled;
            Reached2 = abs(Lum_End2) >= GradientScaled;
            ReachBoth = Reached1 && Reached2;
            
            if(false == Reached1)
                UV1 -= offset;
            if(false == Reached2)
                UV2 += offset;
            
            if(true == ReachBoth)
                break;
        }
    }
    
    float Dist1 = isHorizontal ? (vUV.x - UV1.x) : (vUV.y - UV1.y);
    float Dist2 = isHorizontal ? (UV2.x - vUV.x) : (UV2.y - vUV.y);
    
    bool isDirection = Dist1 < Dist2;
    float DistFinal = min(Dist1, Dist2);
    
    float Edge_Thickness = Dist1 + Dist2;
    
    float PixelOffset = -DistFinal / Edge_Thickness + 0.5f;
    
    bool isLumMidSmaller = Lum_M < Lum_LA;
    
    bool CorrectVariation = ((isDirection ? Lum_End1 : Lum_End2) < 0.f) != isLumMidSmaller;
    
    float FinalOffset = CorrectVariation ? PixelOffset : 0.f;
    
    float Lum_Av = (1.f / 12.f) * (2.f * (Lum_UD + Lum_LR) + Lum_LC + Lum_RC);
    
    float SubPixelOffset1 = saturate(abs(Lum_Av - Lum_M) / Lum_Range);
    float SubPixelOffset2 = (-2.f * SubPixelOffset1 + 3.f) * SubPixelOffset1 * SubPixelOffset1;
    
    float SubPixelOffsetFinal = SubPixelOffset2 * SubPixelOffset2 * 0.75f;
    
    FinalOffset = max(FinalOffset, SubPixelOffsetFinal);
    
    float2 FinalUV = vUV;
    
    if(isHorizontal)
        FinalUV.y += FinalOffset * stepLegth;
    else
        FinalUV.x += FinalOffset * stepLegth;
    
    //SamplerState LinearSampler = sampler_state
    //{
    //    Filter = MIN_MAG_MIP_LINEAR;
    //    AddressU = Clamp;
    //    AddressV = Clamp;
    //};
    
    vColor = inputTexture.SampleLevel(LinearSampler, FinalUV, 0.f);
    
   
    
    outputTexture[pixel] = vColor;
}
