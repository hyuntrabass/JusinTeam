#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

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

//Texture2DMS<vector> g_DiffuseTexture;
// MRT_Object
Texture2D g_DiffuseTexture;
Texture2D g_Normal_Spec_Texture;
Texture2D g_Depth_Velocity_Texture;
Texture2D g_RimMaskTexture;

// MRT_Light
Texture2D g_ShadeTexture;
Texture2D g_SpecularTexture;


Texture2D g_BlurTexture;

Texture2D g_DebugTexture;
Texture2DArray g_DebugArrayTexture;


// 그림자
Texture2DArray g_LightDepthTexture;
matrix g_LightViewMatrix[3];
matrix g_LightProjMatrix[3];
vector g_ClipZ;
bool g_TurnOnShadow;
uint g_hi;


// SSAO
Texture2D g_SSAONoiseNormal;
Texture2D g_SSAOTexture;
matrix g_CamViewMatrix;
matrix g_CamProjMatrix;

// HDR
Texture2D g_Luminance;
Texture2D g_HDRTexture;

// Blurs
Texture2D g_TestBlurTexture;
bool TurnOnSSAO;
bool TurnOnToneMap;
bool TurnOnBloom;
bool TurnOnRim;
SSAO_DESC g_SSAO;
HDR_DESC g_HDR;

vector Get_WorldPos(float2 vTex)
{
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(PointClampSampler, vTex);
    float fViewZ = vDepth_Velocity_Desc.y * g_vCamNF.y;
    
    vector vWorldPos;
    vWorldPos.x = vTex.x * 2.f - 1.f;
    vWorldPos.y = vTex.y * -2.f + 1.f;
    vWorldPos.z = vDepth_Velocity_Desc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos = vWorldPos * fViewZ;
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);
    
    return vWorldPos;
}

void Get_Normal(float2 vTex, out vector vNormal, out float Spec)
{
    vector vNormal_Spec = g_Normal_Spec_Texture.Sample(PointSampler, vTex);
    
    vNormal = normalize(vector(vNormal_Spec.xyz * 2.f - 1.f, 0.f));
    
    Spec = vNormal_Spec.a;
}

vector Get_ViewPos(float2 vTex)
{
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(PointClampSampler, vTex);
    float fViewZ = vDepth_Velocity_Desc.y * g_vCamNF.y;
    
    vector ViewPos;
    ViewPos.x = vTex.x * 2.f - 1.f;
    ViewPos.y = vTex.y * -2.f + 1.f;
    ViewPos.z = vDepth_Velocity_Desc.x;
    ViewPos.w = 1.f;
    
    ViewPos = ViewPos * fViewZ;
    ViewPos = mul(ViewPos, g_ProjMatrixInv);
    
    return ViewPos;
}

float Get_Luminance(float3 vRGB)
{
    return max(dot(vRGB, g_fLuminace), 0.001f);
}

float CalcCascadeShadowFactor(uint lightIndex, vector vLightPos)
{
    float3 ProjPos = vLightPos.xyz / vLightPos.w;
    
    ProjPos.x = (ProjPos.x + 1.f) * 0.5f;
    ProjPos.y = (ProjPos.y - 1.f) * -0.5f;
    
    if(1.f < ProjPos.z)
        return 1.f;
    
    float3 LightUV = float3(ProjPos.x, ProjPos.y, lightIndex);
    
    return g_LightDepthTexture.Sample(LinearSampler, LightUV).r;
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
    
    Output.vColor = g_DebugTexture.Sample(LinearSampler, Input.vTexcoord);
    
    return Output;
}

PS_OUT PS_Main_DebugArray(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;

    Output.vColor = g_DebugArrayTexture.Sample(LinearSampler, float3(Input.vTexcoord, (float) g_hi));
    
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
    
    vector vNormal;
    float Spec;
    
    Get_Normal(Input.vTexcoord, vNormal, Spec);
    
    //Output.vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
    //Output.vShade = g_vLightDiffuse * saturate(ceil(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient); // 카툰
    Output.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient); // 카툰X
    
    vector vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = Spec * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);
    
    //vector vRimMask = g_RimMaskTexture.Sample(LinearSampler, Input.vTexcoord);
    
    //float fRimInensity = dot(vSpecDesc.rgb, vRimMask.rgb);
    
    //Output.vEmissive = vRimMask * fRimInensity;
    
    return Output;
}

PS_OUT_Light PS_Main_Point(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);

    vector vNormal;
    float Spec;
    
    Get_Normal(Input.vTexcoord, vNormal, Spec);
    
    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);
    
    float fAtt = 1.f / (g_vLightAtt.y + g_vLightAtt.z * fDistance + g_vLightAtt.w * (fDistance * fDistance));

    //Output.vShade = fAtt * (g_vLightDiffuse * saturate(ceil(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient));
    Output.vShade = fAtt * (g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient));

    vector vReflect = normalize(reflect(normalize(vLightDir), vNormal));
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = fAtt * (Spec * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f));
    
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
    
    FinalColor = vDiffuse * vShade + vSpecular + vRimMask;
    
    vector vSsaoDesc = g_SSAOTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if (TurnOnSSAO)
        FinalColor.rgb *= vSsaoDesc.rgb;
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    
    vector vDepth_Velocity_Desc = g_Depth_Velocity_Texture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepth_Velocity_Desc.y * g_vCamNF.y;
    
    if (true == g_TurnOnShadow)
    {
        float ClipZ[3];
        ClipZ[0] = g_ClipZ.y;
        ClipZ[1] = g_ClipZ.z;
        ClipZ[2] = g_ClipZ.w;
    
        vector vLightPos;
        for (uint i = 0; i < 3; ++i)
        {
            float Z = ClipZ[i] * (g_vCamNF.y - g_vCamNF.x) - g_vCamNF.y * g_vCamNF.x / g_vCamNF.y;
            vLightPos = mul(vWorldPos, g_LightViewMatrix[i]);
            vLightPos = mul(vLightPos, g_LightProjMatrix[i]);
        
            if (fViewZ <= Z)
            {
                float fLightDepth = CalcCascadeShadowFactor(i, vLightPos);
                if (vLightPos.z - 0.075f > fLightDepth)
                {
                    FinalColor.rgb = FinalColor.rgb * 0.5f;
                }
                break;
            }
        }
    }
    
    float fFogFactor = saturate((g_vFogNF.y - fViewZ) / (g_vFogNF.y - g_vFogNF.x));
    vector vFogColor = g_vFogColor;
    vFogColor.a = 1.f;
    
    if (vWorldPos.y < g_fHellStart)
    {
        //float fHell = (vWorldPos.y + 15.f) / 35.f;
        float fHell = 1.f - ((g_fHellStart - vWorldPos.y) / 20.f);
        fHell = max(fHell, 0.f);
        FinalColor *= vector(fHell, fHell, fHell, 1.f);
        vFogColor *= fHell;
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

//float Get_AO(float2 vTex, float2 plusTex, vector myPos, float3 myNor)
float Get_Occlusion(float distZ)
{
    float occlusion = 0.f;
    
    if (distZ > 0.05f)
    {
        float fadeLength = 2.f - 0.01f;
        
        occlusion = saturate((2.f - distZ) / fadeLength);
    }
    
    return occlusion;
}

PS_OUT PS_Main_SSAO(PS_IN Input)
{
    // 완벽한듯?
    
    PS_OUT Out = (PS_OUT) 0;
    
    vector vNormal;
    float Spec;
    
    Get_Normal(Input.vTexcoord, vNormal, Spec);
    
    float3 MyViewNormal = normalize(mul(vNormal, g_CamViewMatrix).xyz);
    
    float3 P = Get_ViewPos(Input.vTexcoord).xyz;
    
    vector vNoiseNormal = g_SSAONoiseNormal.Sample(LinearSampler, Input.vTexcoord);
    
    vNoiseNormal.xyz = normalize(vNoiseNormal.xyz * 2.f - 1.f);
    
    float OcclusionSum = 0.f;
    
    for (uint i = 0; i < 14; ++i)
    {
        float3 OffsetVector = normalize(g_OffSetVector[i]) * g_Random[i];
        
        float3 Offset = reflect(OffsetVector, vNoiseNormal.xyz);
        
        float flip = sign(dot(Offset, MyViewNormal));
        
        float3 Q = P + flip * 0.25f * Offset;
        
        vector ProjQ = mul(vector(Q, 1.f), g_CamProjMatrix);
        ProjQ /= ProjQ.w;
        
        ProjQ.x = (ProjQ.x + 1.f) * 0.5f;
        ProjQ.y = (ProjQ.y - 1.f) * -0.5f;
        
        float3 R = Get_ViewPos(ProjQ.xy).xyz; //(rz / Q.z) * Q;
        
        float DistZ = P.z - R.z;
        
        float dp = max(dot(MyViewNormal, normalize(R - P)), 0.f);
        
        float Occlusion = dp * Get_Occlusion(DistZ);
        
        OcclusionSum += Occlusion;
    }
    
    OcclusionSum /= 14.f;
    
    float Access = 1.f - OcclusionSum;
    
    Access = pow(Access, 4.f);
    
    Out.vColor = vector(Access, Access, Access, 1.f);
    
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
        float a = 2.51f;
        float b = 0.03f;
        float c = 2.43f;
        float d = 0.59f;
        float e = 0.14f;
        vHDRColor = saturate((vHDRColor * (a * vHDRColor + b)) / (vHDRColor * (c * vHDRColor + d) + e));
        
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
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Debug();
    }

    pass DebugArray
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_DebugArray();
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