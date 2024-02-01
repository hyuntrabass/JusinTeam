#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

Texture2D g_NormalTexture;

Texture2D g_ReflectionTexture; // 반사 텍스처
Texture2D g_RefractionTexture; // 굴절 텍스처

matrix g_ReflectionViewMatrix; // 반사 View Matrix

float g_fReflectionScale;
float g_fRefractionScale;

float g_fWaterPos;

vector g_vCamPos;
float2 g_vCamNF;


struct VS_IN
{
    float3 vPos : POSITION;
    float2 vTex : TEXCOORD0;
};

struct VS_OUT_WATER
{
    vector vPos : SV_POSITION;
    float2 vTex : TEXCOORD0;
    vector vProjPos : PROJPOS;
    vector vReflectionPos : REFLECTION;
    vector vRefractionPos : REFRACTION;
};

VS_OUT_WATER VS_MAIN_Water(VS_IN Input)
{
    VS_OUT_WATER Output = (VS_OUT_WATER) 0;
    
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Output.vPos = mul(vector(Input.vPos, 1.f), matWVP);
    Output.vTex = Input.vTex;
    Output.vProjPos = Output.vPos;
    
    matrix matWRV, matWRVP;
    
    matWRV = mul(g_WorldMatrix, g_ReflectionViewMatrix);
    matWRVP = mul(matWRV, g_ProjMatrix);
        
    Output.vReflectionPos = mul(vector(Input.vPos, 1.f), matWRVP);
    Output.vRefractionPos = Output.vPos;
    
    return Output;
}

struct PS_IN_WATER
{
    vector vPos : SV_POSITION;
    float2 vTex : TEXCOORD0;
    vector vProjPos : PROJPOS;
    vector vReflectionPos : REFLECTION;
    vector vRefractionPos : REFRACTION;
};

struct PS_OUT_WATER
{
    vector vDiffuse : SV_TARGET0;
    vector vNormal : SV_TARGET1;
    vector vDepth : SV_TARGET2;
};

PS_OUT_WATER PS_MAIN_Water(PS_IN_WATER Input)
{
    PS_OUT_WATER Output = (PS_OUT_WATER) 0;
    
    
    vector vNormalDesc;
    vector vNormal;
    
    float2 vWaterTex = Input.vTex + g_fWaterPos;
    
    
    // 반사와 굴절
    vNormalDesc = g_NormalTexture.Sample(LinearSampler, vWaterTex * 8.f);
    vNormal = normalize(vector(vNormalDesc.xyz * 2.f - 1.f, 0.f) * -1.f);
    vNormal.a = 0.f;
    
    //vNormal = normalize(mul(vNormal, g_WorldMatrix) * -1.f);
    
    float2 vReflectionTexcoord;
    
    vReflectionTexcoord.x = Input.vReflectionPos.x / Input.vReflectionPos.w / 2.f + 0.5f;
    vReflectionTexcoord.y = -Input.vReflectionPos.y / Input.vReflectionPos.w / 2.f + 0.5f;
    
    //float2 vRefractionTexcoord;
    
    //vRefractionTexcoord.x = Input.vRefractionPos.x / Input.vRefractionPos.w / 2.f + 0.5f;
    //vRefractionTexcoord.y = -Input.vRefractionPos.y / Input.vRefractionPos.w / 2.f + 0.5f;
    
    vReflectionTexcoord += vNormal.xy * g_fReflectionScale;
    //vRefractionTexcoord += vNormal.xy * g_fRefractionScale;
    
    vector vReflectionDiffuse;
    
    vReflectionDiffuse = g_ReflectionTexture.Sample(LinearMirrorSampler, vReflectionTexcoord);
    
    //vector vRefractionDiffuse;
    //vRefractionDiffuse = g_RefractionTexture.Sample(LinearMirrorSampler, vRefractionTexcoord);
    
    // 프레넬(반사율과 투과율 계산)

    //vector vLook = normalize(Input.vProjPos - g_vCamPos);
    
    //float fFresnel = 0.02f + 0.97f * pow((1.f - dot(vLook, vNormal)), 5.f);
    
    //vector vMergeDiffuse = lerp(vReflectionDiffuse, vRefractionDiffuse, fFresnel);
    
   // vector vMergeDiffuse = (1.f - fFresnel) * vRefractionDiffuse + fFresnel * vReflectionDiffuse;
    //float4 vMergeDiffuse = vRefractionDiffuse * (1 - fFresnel) * vRefractionDiffuse.a * vReflectionDiffuse.a + vReflectionDiffuse * fFresnel * vReflectionDiffuse.a * vRefractionDiffuse.a;
    //vMergeDiffuse = saturate(vMergeDiffuse * float4(0.95f, 1.00f, 1.05f, 1.0f) + float4(0.15f, 0.15f, 0.15f, 0.0f));
    
    Output.vDiffuse = vReflectionDiffuse;
    Output.vNormal = vNormal;
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_vCamNF.y, 0.f, 0.f);
    
    return Output;
}

// 물 쉐이더
technique11 DefaultTechniqueShader_VtxTex
{
    pass Water
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN_Water();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_MAIN_Water();
    }
}