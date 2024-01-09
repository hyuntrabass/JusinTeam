#include "Engine_Shader_Define.hlsli"

matrix g_BoneMatrices[256];

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_NoiseTexture;

vector g_vCamPos;
float g_fCamFar;
float g_fLightFar;
float g_fDissolveRatio;

bool g_HasNorTex;
bool g_bSelected = false;

struct VS_IN
{
    float3 vPos : Position;
    float3 vNor : Normal;
    float2 vTex : Texcoord0;
    float3 vTan : Tangent;
    uint4 vBlendIndices : BlendIndex;
    float4 vBlendWeight : BlendWeight;
};

struct VS_OUT
{
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
};

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    float fW = 1.f - (Input.vBlendWeight.x + Input.vBlendWeight.y + Input.vBlendWeight.z);
    matrix Bone = g_BoneMatrices[Input.vBlendIndices.x] * Input.vBlendWeight.x +
    g_BoneMatrices[Input.vBlendIndices.y] * Input.vBlendWeight.y +
    g_BoneMatrices[Input.vBlendIndices.z] * Input.vBlendWeight.z +
    g_BoneMatrices[Input.vBlendIndices.w] * fW;
    
    vector vPos = mul(vector(Input.vPos, 1.f), Bone);
    vector vNor = mul(vector(Input.vNor, 0.f), Bone);
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Output.vPos = mul(vPos, matWVP);
    Output.vNor = normalize(mul(vNor, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    
    return Output;
}

VS_OUT VS_Main_OutLine(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    float fW = 1.f - (Input.vBlendWeight.x + Input.vBlendWeight.y + Input.vBlendWeight.z);
    matrix Bone = g_BoneMatrices[Input.vBlendIndices.x] * Input.vBlendWeight.x +
    g_BoneMatrices[Input.vBlendIndices.y] * Input.vBlendWeight.y +
    g_BoneMatrices[Input.vBlendIndices.z] * Input.vBlendWeight.z +
    g_BoneMatrices[Input.vBlendIndices.w] * fW;
    
    vector vPos = mul(vector(Input.vPos, 1.f), Bone);
    vector vNor = mul(vector(Input.vNor, 0.f), Bone);
    
    float fDist = length(g_vCamPos - mul(vPos, g_WorldMatrix));
    
    float fThickness = clamp(fDist / 300.f, 0.001f, 0.05f);
    
    vPos += normalize(vNor) * fThickness;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
	
    Output.vPos = mul(vPos, matWVP);
    Output.vNor = normalize(mul(vNor, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
	
    return Output;
}

struct PS_IN
{
    vector vPos : SV_Position;
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
};

struct PS_OUT
{
    vector vColor : SV_Target0;
};

struct PS_OUT_DEFERRED
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
};

PS_OUT_DEFERRED PS_Main(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + vector(0.5f, 0.f, 0.f, 0.f) * g_bSelected;
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = mul(normalize(vNormal), WorldMatrix);
    }
    else
    {
        vNormal = Input.vNor.xyz;
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Output;
}

PS_OUT_DEFERRED PS_Main_OutLine(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vLook = g_vCamPos - Input.vWorldPos;
    float DotProduct = dot(normalize(vLook), normalize(Input.vNor));
    if (DotProduct > 0.f)
    {
        discard;
    }
    
    Output.vDiffuse = vector(0.f, 0.f, 0.f, 1.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Output;
}

vector PS_Main_Shadow(PS_IN Input) : SV_Target0
{
    vector Output = (vector) 0;
    
    Output.x = Input.vProjPos.w / g_fLightFar;
    
    return Output;
}

PS_OUT_DEFERRED PS_Main_Dissolve(PS_IN Input)
{
    PS_OUT_DEFERRED Output = (PS_OUT_DEFERRED) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex) + vector(0.5f, 0.f, 0.f, 0.f) * g_bSelected;
    float fDissolve = g_NoiseTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
    
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
    
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
    
        vNormal = mul(normalize(vNormal), WorldMatrix);
    }
    else
    {
        vNormal = Input.vNor.xyz;
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    
    return Output;
}

technique11 DefaultTechniqueShader_VtxNorTex
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }

    pass OutLine
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main_OutLine();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_OutLine();
    }

    pass Shadow
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_None, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Shadow();
    }

    pass Dissolve
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Dissolve();
    }
};