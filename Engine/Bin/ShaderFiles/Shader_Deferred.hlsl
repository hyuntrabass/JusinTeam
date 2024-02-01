#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;
matrix g_LightViewMatrix, g_LightProjMatrix;

vector g_vLightDir;
vector g_vLightPos;
vector g_vLightAtt;
vector g_vLightDiffuse;
vector g_vLightAmbient;
vector g_vLightSpecular;

vector g_vCamPosition;
float2 g_vCamNF;
float g_fLightFar;
float g_fHellStart;

float2 g_vFogNF;
vector g_vFogColor;

Texture2D g_DiffuseTexture;
Texture2D g_NormalTexture;
Texture2D g_ObjectSpecTexture;
Texture2D g_ShadeTexture;
Texture2D g_SpecularTexture;
Texture2D g_DepthTexture;
Texture2D g_LightDepthTexture;
Texture2D g_BlurTexture;
Texture2D g_Texture;

// 원명
Texture2D g_VelocityTexture;
Texture2D g_RimMaskTexture;
Texture2D g_SSAONoiseNormal;
Texture2D g_SSAOTexture;
Texture2D g_HDRTexture;
Texture2D g_Luminance;
Texture2D g_TestBlurTexture;
bool TurnOnSSAO;
bool TurnOnToneMap;
bool TurnOnBloom;
bool TurnOnRim;
SSAO_DESC g_SSAO;
HDR_DESC g_HDR;

vector Get_WorldPos(float2 vTex)
{
    vector vDepthDesc = g_DepthTexture.Sample(PointClampSampler, vTex);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector vWorldPos;
    vWorldPos.x = vTex.x * 2.f - 1.f;
    vWorldPos.y = vTex.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    return vWorldPos;
}

vector Get_Normal(float2 vTex)
{
    vector vNormalDesc = g_NormalTexture.Sample(PointClampSampler, vTex);
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    return normalize(vNormal);
}

vector Get_ViewPos(float2 vTex)
{
    vector vDepthDesc = g_DepthTexture.Sample(PointClampSampler, vTex);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector ViewPos;
    ViewPos.x = vTex.x * 2.f - 1.f;
    ViewPos.y = vTex.y * -2.f + 1.f;
    ViewPos.z = vDepthDesc.x;
    ViewPos.w = 1.f;
    
    ViewPos = ViewPos * fViewZ;
    ViewPos = mul(ViewPos, g_ProjMatrixInv);
    
    return ViewPos;
}

float Get_Luminance(float3 vRGB)
{
    return dot(vRGB, g_fLuminace);
}

struct VS_IN
{
    float3 vPos : Position;
    float2 vTexcoord : Texcoord0;
};

struct VS_OUT
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vTexcoord = Input.vTexcoord;
    
    
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

PS_OUT PS_Main_Debug(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = g_Texture.Sample(LinearSampler, Input.vTexcoord);
    
    return Output;
}

struct PS_OUT_Light
{
    vector vShade : SV_Target0;
    vector vSpecular : SV_Target1;
    vector vEmissive : SV_Target2;
};

PS_OUT_Light PS_Main_Directional(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vNormal = Get_Normal(Input.vTexcoord);
    
    //Output.vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
    //Output.vShade = g_vLightDiffuse * saturate(ceil(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient); // 카툰
    Output.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient); // 카툰X
    
    vector vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    vector vSpecDesc = g_ObjectSpecTexture.Sample(PointSampler, Input.vTexcoord);
    
    Output.vSpecular = vSpecDesc * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);
    
    //vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float fRimInensity = dot(vSpecDesc.rgb, vRimMask.rgb);
    
    //Output.vEmissive = vRimMask * fRimInensity;
    
    return Output;
}

PS_OUT_Light PS_Main_Point(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector vNormal = Get_Normal(Input.vTexcoord);
    
    vector vSpecDesc = g_ObjectSpecTexture.Sample(PointSampler, Input.vTexcoord);
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = 1.f / (g_vLightAtt.y + g_vLightAtt.z * fDistance + g_vLightAtt.w * (fDistance * fDistance));

    //Output.vShade = fAtt * (g_vLightDiffuse * saturate(ceil(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient));
    Output.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient));

    vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = fAtt * (vSpecDesc * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f));
    
    //vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float fRimInensity = dot(vSpecDesc.rgb, vRimMask.rgb);
    
    //Output.vEmissive = fAtt * vRimMask * fRimInensity;
    
    return Output;
}

PS_OUT PS_Main_Water(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector FinalColor = 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTexcoord);
    if(vDiffuse.a == 0.f)
        discard;
    
    vDiffuse.rgb = pow(vDiffuse.rgb, 2.2f);
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, Input.vTexcoord);
    
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, Input.vTexcoord);
    
    FinalColor.rgb = vDiffuse.rgb * vShade.rgb; //+ vSpecular;
    FinalColor.a = vDiffuse.a;
    Output.vColor = FinalColor;
    
    return Output;
}

PS_OUT PS_Main_Deferred(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector FinalColor = 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTexcoord);
    if (vDiffuse.a == 0.f)
    {
        discard;
    }
    
    vDiffuse.rgb = pow(vDiffuse.rgb, 2.2f);
    
    vector vShade = g_ShadeTexture.Sample(LinearSampler, Input.vTexcoord);
    vShade.a = 1.f;
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, Input.vTexcoord);
    vSpecular.a = 0.f;
    
    vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    vRimMask.a = 0.f;
    
    if (TurnOnRim)
        vRimMask = 0.f;
    
    FinalColor = vDiffuse * vShade + vSpecular + vRimMask;
    
    vector vSsaoDesc = g_SSAOTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if (TurnOnSSAO)
        FinalColor.rgb *= (1.f - vSsaoDesc.rgb);
    
    //if(true == TurnOnThunder)
    //    FinalColor = pow(vShade, 50.f) + vDiffuse * 0.1f;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    float fFogFactor = saturate((g_vFogNF.y - fViewZ) / (g_vFogNF.y - g_vFogNF.x));
    vector vFogColor = g_vFogColor;
    vFogColor.a = 1.f;
    
    float4 vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    if (vWorldPos.y < g_fHellStart)
    {
        float fHell = (vWorldPos.y + 15.f) / 35.f;
        FinalColor *= vector(fHell, fHell, fHell, 1.f);
        vFogColor *= fHell;
    }
    
    vWorldPos = mul(vWorldPos, g_LightViewMatrix);
    vWorldPos = mul(vWorldPos, g_LightProjMatrix);
    
    float2 vLightDepthUV;
    vLightDepthUV.x = vWorldPos.x / vWorldPos.w * 0.5f + 0.5f;
    vLightDepthUV.y = vWorldPos.y / vWorldPos.w * -0.5f + 0.5f;
    
    vector vLightDepthDesc = g_LightDepthTexture.Sample(LinearClampSampler, vLightDepthUV);
    
    float fLightDepth = vLightDepthDesc.x * g_fLightFar;
    
    if (vWorldPos.w - 0.001f > fLightDepth)
    {
        FinalColor.xyz = FinalColor.xyz * 0.5f;
    }

    FinalColor = lerp(fFogFactor * FinalColor, vFogColor, (1.f - fFogFactor));
    //FinalColor = fFogFactor * FinalColor + (1.f - fFogFactor) * vFogColor;
    
    Output.vColor = FinalColor;
    
    return Output;
}

PS_OUT PS_Main_Blur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = vector(0.f, 0.f, 0.f, 0.f);
    
    Output.vColor = g_BlurTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float2 fTexelSize = 1.f / float2(g_fScreenWidth, g_fScreenHeight);
    
    //for (int y = -10; y <= 10; ++y)
    //{
    //    for (int x = -10; x <= 10; ++x)
    //    {
    
    //        Output.vColor += g_BlurTexture.Sample(LinearSampler, Input.vTexcoord + float2(x, y) * fTexelSize) * 1.f / 441.f;
    //    }
    //}
    
    return Output;
}

float Get_AO(float2 vTex, float2 plusTex, vector myPos, float3 myNor)
//float Get_Occlusion(float distZ)
{
    vector diff = Get_WorldPos(vTex + plusTex) - myPos;
    vector v = normalize(diff);
    float d = length(diff) * g_SSAO.fScale;
    
    float final = max(0.f, dot(myNor, v.xyz) - g_SSAO.fBias) * (1.f / (1.f + d)) * g_SSAO.fIntensity;
    
    return final;
    
    //float occlusion = 0.f;
    
    //if (distZ > 0.05f)
    //{
    //    float fadeLength = 2.f - 0.2f;
        
    //    occlusion = saturate((2.f - distZ) / fadeLength);
    //}
    
    //return occlusion;
}

PS_OUT PS_Main_SSAO(PS_IN Input)
{
    // 완벽하지 않습니당 임시용
    
    PS_OUT Out = (PS_OUT) 0;
    
    //vector MyNormal = Get_Normal(Input.vTexcoord);
    
    //vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    
    //vector MyViewPos = Get_ViewPos(Input.vTexcoord);
    
    //float3 RandVec = 2.f * g_SSAONoiseNormal.Sample(LinearSampler, 4.f * Input.vTexcoord).rgb - 1.f;
    
    //float OcclusionSum = 0.f;
    
    //for (uint i = 0; i < 16; ++i)
    //{
    //    float3 Offset = reflect(g_OffSetVector[i], RandVec);
        
    //    float flip = sign(dot(Offset, MyNormal.xyz));
        
    //    float3 q = MyViewPos.xyz + flip * g_SSAO.fRadius * Offset;
        
    //    //float4 ProjQ = mul(float4(q, 1.f), g_CamProjMatrix);
    //    //ProjQ /= ProjQ.w;
        
    //    float3 r = Get_ViewPos(q.xy);
        
    //    float DistZ = MyViewPos.z - r.z;
    //    float dp = max(dot(MyNormal.xyz, normalize(r - MyViewPos.xyz)), 0.f);
    //    float Occlusion = dp * Get_Occlusion(DistZ);

    //    OcclusionSum += Occlusion;
    //}
    
    //OcclusionSum /= 16.f;
    
    //float occl = saturate(pow(OcclusionSum, 4.f));
    
    //Out.vColor = vector(occl, occl, occl, 1.f);
    
    //return Out;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector MyWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector MyNormal = Get_Normal(Input.vTexcoord);
    
    float ssao = 0.f;
    float fRadius = g_SSAO.fRadius / fViewZ;
    for (uint i = 0; i < 16; ++i)
    {
        vector vRandomNormal = g_SSAONoiseNormal.Sample(LinearSampler, Input.vTexcoord + (0.5f * i));
        
        float3 vReflectedLight = reflect(normalize(MyNormal.xyz), normalize(vRandomNormal.xyz));
        
        float Dot = dot(normalize(MyNormal.xyz), normalize(vReflectedLight));
        
        if (0.f > Dot)
        {
            vReflectedLight = reflect(normalize(MyNormal.xyz), normalize(-vRandomNormal.xyz));
        }
        
        float2 vReflect = vReflectedLight.xy * fRadius;
        
        ssao += Get_AO(Input.vTexcoord, vReflect, MyWorldPos, MyNormal.xyz);
    }
    ssao = ssao / 16.f;
    
    ssao = saturate(pow(ssao, 2.f));
    
    Out.vColor = vector(ssao, ssao, ssao, 1.f);
    
    return Out;
    
}

PS_OUT PS_Main_HDR(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vColor = g_HDRTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if(0.f == vColor.a)
        discard;
    
    float3 vHDRColor = vColor.rgb;
    
    if (true == TurnOnBloom)
    {
        vector vBlur = g_TestBlurTexture.Sample(LinearSampler, Input.vTexcoord);
        
        vHDRColor += vBlur.rgb;
    }
    
    if (true == TurnOnToneMap)
    {
        //if (ChangeToneMap == 0)
        //{
        //    //SimpleReinhardToneMapping
        //    vHDRColor *= 1.5f / (1.f + vHDRColor / 1.5f);
            
        //}
        //else if (ChangeToneMap == 1)
        //{
        //    //LuminanceBasedReinhardTonMapping
        //    float fLum = Get_Luminance(vHDRColor);
        //    float ToneMappedLum = fLum / (1.f + fLum);
        //    vHDRColor *= ToneMappedLum / fLum;
            
        //}
        //else if (ChangeToneMap == 2)
        //{
        //    //WhitePreservingLuminanceBasedReinhardToneMapping
        //    float White = 2.f;
        //    float fLum = Get_Luminance(vHDRColor);
        //    float ToneMappedLum = fLum * (1.f + fLum / (White * White)) / (1.f + fLum);
        //    vHDRColor *= ToneMappedLum / fLum;
            
        //}
        //else if (ChangeToneMap == 3)
        //{
        //    //RomBinDaHouseToneMapping
        //    vHDRColor = exp(-1.f / (2.72f * vHDRColor + 0.15f));
            
        //}
        //else if (ChangeToneMap == 4)
        //{
        //    //FilmicToneMapping
        //    vHDRColor = max(0.f, vHDRColor - 0.004f);
        //    vHDRColor = (vHDRColor * (62.f * vHDRColor + 0.5f)) / (vHDRColor * (6.2f * vHDRColor + 1.7f) + 0.06f);
        //}
        //else if (ChangeToneMap == 5)
        //{
        //    // Uncharted2ToneMapping
        //    float A = 0.15f;
        //    float B = 0.5f;
        //    float C = 0.1f;
        //    float D = 0.2f;
        //    float E = 0.02f;
        //    float F = 0.3f;
        //    float W = 11.2f;
        //    float exposure = 2.f;
            
        //    vHDRColor *= exposure;
        //    vHDRColor = ((vHDRColor * (A * vHDRColor + C * B) + D * E) / (vHDRColor * (A * vHDRColor + B) + D * F)) - E / F;
        //    float White = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
        //    vHDRColor /= White;

        //}
        //else if (ChangeToneMap == 6)
        //{
        //    float fAvgLum = g_Luminance.Sample(PointClampSampler, float2(0.f, 0.f)).r;
        
        //    float fLScale = Get_Luminance(vHDRColor);
        //    fLScale *= g_HDR.fMiddleGray / fAvgLum;
        //    fLScale = (fLScale + fLScale * fLScale / g_HDR.fLumWhiteSqr) / (1.f + fLScale);
        
        //    vHDRColor *= fLScale;
        //}
        //else if (ChangeToneMap == 7)
        //{
            float a = 2.51f;
            float b = 0.03f;
            float c = 2.43f;
            float d = 0.59f;
            float e = 0.14f;
            vHDRColor = saturate((vHDRColor * (a * vHDRColor + b)) / (vHDRColor * (c * vHDRColor + d) + e));
        //}
        
    }
    
    
    vHDRColor = pow(vHDRColor, 1.f / 2.2f);
        
    Output.vColor = vector(vHDRColor, vColor.a);
    
    return Output;
}

technique11 DefaultTechnique
{
    pass Debug
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Debug();
    }

    pass Light_Directional
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Directional();
    }

    pass Light_Point
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Point();
    }

    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Water();
    }

    pass Deferrd
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Deferred();
    }

    pass Blur
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_OnebyOne, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Blur();
    }

    pass SSAO
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_SSAO();
    }

    pass HDR
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_HDR();
    }


};