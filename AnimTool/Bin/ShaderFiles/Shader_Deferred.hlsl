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
float g_fScreenWidth;
float g_fScreenHeight;
float g_fHellStart;

float2 g_vFogNF;

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
Texture2D g_SSAOTexture;
Texture2D g_HDRTexture;
Texture2D g_TestBlurTexture;
bool TurnOnSSAO;
bool TurnOnToneMap;
bool TurnOnBlur;
SSAO_DESC g_SSAO;


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
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, vTex);
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
    return vNormal;
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
    float2 vNearTexcoord1 : Texcoord1;
    float2 vNearTexcoord2 : Texcoord2;
    float2 vNearTexcoord3 : Texcoord3;
    float2 vNearTexcoord4 : Texcoord4;
    float2 vNearTexcoord5 : Texcoord5;
    float2 vNearTexcoord6 : Texcoord6;
    float2 vNearTexcoord7 : Texcoord7;
    float2 vNearTexcoord8 : Texcoord8;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matrix matWVP = mul(matWV, g_ProjMatrix);

    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vTexcoord = Input.vTexcoord;
    
    float2 fTexelSize = float2(1.f / g_fScreenWidth, 1.f / g_fScreenHeight);
    
    Output.vNearTexcoord4 = Input.vTexcoord + float2(0.f, fTexelSize.y * +1.f);
    Output.vNearTexcoord6 = Input.vTexcoord + float2(fTexelSize.x * +1.f, fTexelSize.y * +1.f);
    Output.vNearTexcoord2 = Input.vTexcoord + float2(fTexelSize.x * +1.f, 0.f);
    Output.vNearTexcoord5 = Input.vTexcoord + float2(fTexelSize.x * +1.f, fTexelSize.y * -1.f);
    Output.vNearTexcoord3 = Input.vTexcoord + float2(0.f, fTexelSize.y * -1.f);
    Output.vNearTexcoord8 = Input.vTexcoord + float2(fTexelSize.x * -1.f, fTexelSize.y * -1.f);
    Output.vNearTexcoord7 = Input.vTexcoord + float2(fTexelSize.x * -1.f, fTexelSize.y * +1.f);
    Output.vNearTexcoord1 = Input.vTexcoord + float2(fTexelSize.x * -1.f, 0.f);
    
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    float2 vTexcoord : Texcoord0;
    float2 vNearTexcoord1 : Texcoord1;
    float2 vNearTexcoord2 : Texcoord2;
    float2 vNearTexcoord3 : Texcoord3;
    float2 vNearTexcoord4 : Texcoord4;
    float2 vNearTexcoord5 : Texcoord5;
    float2 vNearTexcoord6 : Texcoord6;
    float2 vNearTexcoord7 : Texcoord7;
    float2 vNearTexcoord8 : Texcoord8;
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
};

PS_OUT_Light PS_Main_Directional(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vNormal = Get_Normal(Input.vTexcoord);
    
    vector vSpecDesc = g_ObjectSpecTexture.Sample(PointSampler, Input.vTexcoord);
    
    //Output.vShade = max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f);
    //Output.vShade = g_vLightDiffuse * saturate(ceil(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) * 2.f) / 2.f + g_vLightAmbient); // 카툰
    Output.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, normalize(vNormal)), 0.f) + g_vLightAmbient); // 카툰X
    
    vector vReflect = normalize(reflect(normalize(g_vLightDir), vNormal));
    
    vector vWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector vLook = vWorldPos - g_vCamPosition;
    
    Output.vSpecular = vSpecDesc * g_vLightSpecular * pow(saturate(dot(normalize(vLook) * -1.f, vReflect)), 30.f);
    
    return Output;
}

PS_OUT_Light PS_Main_Point(PS_IN Input)
{
    PS_OUT_Light Output = (PS_OUT_Light) 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, Input.vTexcoord);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector vNormal = vector(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    
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
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, Input.vTexcoord);
    
    FinalColor = vDiffuse * vShade + vSpecular;
    
    //if(true == TurnOnThunder)
    //    FinalColor = pow(vShade, 50.f) + vDiffuse * 0.1f;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector vFogColor = vector(0.9f, 0.9f, 0.9f, 1.f);
    float fFogFactor = saturate((g_vFogNF.y - fViewZ) / (g_vFogNF.y - g_vFogNF.x));
    
    
    vector vSsaoDesc = g_SSAOTexture.Sample(LinearSampler, Input.vTexcoord);
    
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

    FinalColor = fFogFactor * FinalColor + (1.f - fFogFactor) * vFogColor;
    
    if(TurnOnSSAO)
        FinalColor *= vSsaoDesc;
    
    Output.vColor = FinalColor;
    
    return Output;
}

PS_OUT PS_Main_Blur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    Output.vColor = vector(0.f, 0.f, 0.f, 0.f);
    
    float2 fTexelSize = 1.f / float2(g_fScreenWidth, g_fScreenHeight);
    
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
{
    vector diff = Get_WorldPos(vTex + plusTex) - myPos;
    vector v = normalize(diff);
    float d = length(diff) * g_SSAO.fScale;
    
    float final = max(0.f, dot(myNor, v.xyz) - g_SSAO.fBias) * (1.f / (1.f + d)) * g_SSAO.fIntensity;
    
    return final;
}

PS_OUT PS_Main_SSAO(PS_IN Input)
{
    // 완벽하지 않습니당 임시용
    
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Input.vTexcoord);
    float fViewZ = vDepthDesc.y * g_vCamNF.y;
    
    vector MyWorldPos = Get_WorldPos(Input.vTexcoord);
    
    vector MyNormal = Get_Normal(Input.vTexcoord);
    
    float ssao = 0.f;
    float fRadius = g_SSAO.fRadius / fViewZ;
    for (uint i = 0; i < 16; ++i)
    {
        float2 vReflect = reflect(normalize(MyNormal), vector(normalize(g_vRandom[i]), 0.f)).xy * fRadius;
        
        ssao += Get_AO(Input.vTexcoord, vReflect, MyWorldPos, MyNormal.xyz);
        
    }
    ssao = ssao / 16.f;
    
    ssao = 1.f - ssao;
    Out.vColor = vector(ssao, ssao, ssao, 1.f);
    
    return Out;
}

PS_OUT PS_Main_HDR(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vColor = g_HDRTexture.Sample(LinearSampler, Input.vTexcoord);
    
    vector vBlur = g_TestBlurTexture.Sample(LinearSampler, Input.vTexcoord);
    
    if(true == TurnOnBlur)
        vColor.rgb += vBlur.rgb;
    
    if (true == TurnOnToneMap)
    {
        float a = 2.51f;
        float b = 0.03f;
        float c = 2.43f;
        float d = 0.59f;
        float e = 0.14f;
        vColor.rgb = saturate((vColor.rgb * (a * vColor.rgb + b)) / (vColor.rgb * (c * vColor.rgb + d) + e));
    }
    
    vColor.rgb = pow(vColor.rgb, 1.f / 2.2f);
        
    Output.vColor = vColor;
    
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