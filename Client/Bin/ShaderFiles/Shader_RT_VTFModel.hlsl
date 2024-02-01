#include "Engine_Shader_Define.hlsli"

matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_DiffuseTexture;
texture2D g_NormalTexture;
texture2D g_SpecTexture;
texture2D g_DissolveTexture;
vector g_vColor = { 1.f, 1.f, 1.f, 0.f };
float g_fCamFar;
vector g_vCamPos;
float g_fDissolveRatio;

bool g_HasNorTex;
bool g_HasSpecTex;

matrix g_OldWorldMatrix, g_OldViewMatrix;

Texture2DArray g_BoneTexture;

Texture2DArray g_OldBoneTexture;

vector g_RimColor;

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
    float4 vDir : DIRECTION;
};

matrix Get_BoneMatrix(VS_IN Input, Texture2DArray BoneTexture)
{
    float BoneIndices[4] = { Input.vBlendIndices.x, Input.vBlendIndices.y, Input.vBlendIndices.z, Input.vBlendIndices.w };
   
    float fBoneWeights[4] = { Input.vBlendWeight.x, Input.vBlendWeight.y, Input.vBlendWeight.z, Input.vBlendWeight.w };

    vector BoneVec[4];
    matrix Bone = 0;
    
    matrix BoneMatrix = 0;
    
    for (uint i = 0; i < 4; ++i)
    {
        BoneVec[0] = BoneTexture.Load(int4(BoneIndices[i] * 4 + 0, 0, 0, 0));
        BoneVec[1] = BoneTexture.Load(int4(BoneIndices[i] * 4 + 1, 0, 0, 0));
        BoneVec[2] = BoneTexture.Load(int4(BoneIndices[i] * 4 + 2, 0, 0, 0));
        BoneVec[3] = BoneTexture.Load(int4(BoneIndices[i] * 4 + 3, 0, 0, 0));
        
        Bone = matrix(BoneVec[0], BoneVec[1], BoneVec[2], BoneVec[3]);
        
        BoneMatrix += mul(Bone, fBoneWeights[i]);
    }

    return BoneMatrix;
}

VS_OUT VS_Main(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix BoneMatrix = Get_BoneMatrix(Input, g_BoneTexture);
    
    vector vPos = mul(vector(Input.vPos, 1.f), BoneMatrix);
    vector vNormal = mul(vector(Input.vNor, 0.f), BoneMatrix);
    
    Output.vPos = mul(vPos, matWVP);
    Output.vNor = normalize(mul(vNormal, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vDir = 0.f;
    
    return Output;
}


VS_OUT VS_Motion_Blur(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
	
    matrix matWV, matWVP;
    
    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    matrix matOldWV, matOldWVP;
    
    matOldWV = mul(g_OldWorldMatrix, g_OldViewMatrix);
    matOldWVP = mul(matOldWV, g_ProjMatrix);
    
    matrix BoneMatrix = Get_BoneMatrix(Input, g_BoneTexture);
    
    vector vNew = mul(vector(Input.vPos, 1.f), BoneMatrix);
    
    matrix OldBoneMatrix = Get_BoneMatrix(Input, g_OldBoneTexture);
    
    vector vOld = mul(vector(Input.vPos, 1.f), OldBoneMatrix);
    
    vector vNormal = mul(vector(Input.vNor, 0.f), BoneMatrix);
    
    vector vOldPos = mul(vOld, matOldWVP);
    vector vNewPos = mul(vNew, matWVP);
    
    vector vDir = vNewPos - vOldPos;
    vector vCalNor = mul(vNormal, matWV);
    
    float a = dot(normalize(vDir), normalize(vCalNor));
    
    vector vPos;
    if(a<0.f)
        vPos = vOldPos;
    else
        vPos = vNewPos;
    
    float2 velocity = (vNewPos.xy / vNewPos.w) - (vOldPos.xy / vOldPos.w);
    
    //float S = (1.f / g_fDeltaTime) * 1.f;
    
    //velocity = (S / 5.f) * velocity;
    
    vector vCalDir;
    vCalDir.xy = velocity * 0.5f;
    vCalDir.y *= -1.f;
    vCalDir.z = vPos.z;
    vCalDir.w = vPos.w;
    
    Output.vPos = vPos;
    Output.vNor = normalize(mul(vNormal, g_WorldMatrix));
    Output.vTex = Input.vTex;
    Output.vWorldPos = mul(vector(Input.vPos, 1.f), g_WorldMatrix);
    Output.vProjPos = Output.vPos;
    Output.vTangent = normalize(mul(vector(Input.vTan, 0.f), g_WorldMatrix)).xyz;
    Output.vBinormal = normalize(cross(Output.vNor.xyz, Output.vTangent));
    Output.vDir = vCalDir;
    
    return Output;
}

VS_OUT VS_Main_OutLine(VS_IN Input)
{
    VS_OUT Output = (VS_OUT) 0;
    
    matrix matWV, matWVP;
    
    matrix Bone = Get_BoneMatrix(Input, g_BoneTexture);
    
    vector vPos = mul(vector(Input.vPos, 1.f), Bone);
    vector vNor = mul(vector(Input.vNor, 0.f), Bone);
    
    float fDist = length(g_vCamPos - mul(vPos, g_WorldMatrix));
    
    float fThickness = clamp(fDist / g_fCamFar, 0.001f, 0.05f);
    
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
    vector vPos : SV_Position; // == float4
    vector vNor : Normal;
    float2 vTex : Texcoord0;
    vector vWorldPos : Texcoord1;
    vector vProjPos : Texcoord2;
    float3 vTangent : Tangent;
    float3 vBinormal : Binormal;
    float4 vDir : DIRECTION;
};

struct PS_OUT
{
    vector vDiffuse : SV_Target0;
    vector vNormal : SV_Target1;
    vector vDepth : SV_Target2;
    vector vSpecular : SV_Target3;
    vector vVelocity : SV_Target4;
    vector vRimMask : SV_Target5;
};

PS_OUT PS_Main(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    if (0.3f > vMtrlDiffuse.a)
    {
        discard;
    }
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
        
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
        
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vSpecular = vector(0.f, 0.f, 0.f, 0.f);
    if (g_HasSpecTex)
    {
        vector vSpecDesc = g_SpecTexture.Sample(LinearSampler, Input.vTex);
        vSpecDesc = vector(vSpecDesc.b, vSpecDesc.b, vSpecDesc.b, vSpecDesc.a);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    Output.vVelocity = 0.f;
    Output.vRimMask = 0.f;
    
    return Output;
}

PS_OUT PS_Main_Dissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    if (0.3f > vMtrlDiffuse.a)
    {
        discard;
    }
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
        
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
        
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vSpecular = vector(0.f, 0.f, 0.f, 0.f);
    if (g_HasSpecTex)
    {
        vector vSpecDesc = g_SpecTexture.Sample(LinearSampler, Input.vTex);
        vSpecDesc = vector(vSpecDesc.b, vSpecDesc.b, vSpecDesc.b, vSpecDesc.a);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    Output.vVelocity = 0.f;
    Output.vRimMask = 0.f;
    
    return Output;
}

PS_OUT PS_Main_LerpDissolve(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    if (g_vColor.a > 0.f)
    {
        vMtrlDiffuse.xyz = lerp(vMtrlDiffuse, g_vColor, 0.3f).xyz;
    }
    if (0.3f > vMtrlDiffuse.a)
    {
        discard;
    }
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
        
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
        
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vSpecular = vector(0.f, 0.f, 0.f, 0.f);
    if (g_HasSpecTex)
    {
        vector vSpecDesc = g_SpecTexture.Sample(LinearSampler, Input.vTex);
        vSpecDesc = vector(vSpecDesc.b, vSpecDesc.b, vSpecDesc.b, vSpecDesc.a);
    }
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    Output.vVelocity = 0.f;
    Output.vRimMask = 0.f;
    
    return Output;
}

PS_OUT PS_Main_Rim(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    
    if (0.3f > vMtrlDiffuse.a)
    {
        discard;
    }
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
        
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
        
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
        
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vSpecular = vector(0.1f, 0.1f, 0.1f, 0.1f);
    if (g_HasSpecTex)
    {
        vector vSpecDesc = g_SpecTexture.Sample(LinearSampler, Input.vTex);
        vSpecDesc = vector(vSpecDesc.b, vSpecDesc.b, vSpecDesc.b, vSpecDesc.a);
    }
    
    float3 vToCamera = normalize(g_vCamPos - Input.vWorldPos).xyz;

    //fRim = 1.f - saturate(dot(vViewNormal, vToCamera));
    float fRim = smoothstep(0.5f, 1.f, 1.f - max(0.f, dot(vNormal, vToCamera)));
    
    vector vRimColor = g_RimColor * fRim;
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    Output.vVelocity = 0.f;
    Output.vRimMask = vRimColor;
    
    return Output;
}

PS_OUT PS_Motion_Blur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    if (0.3f > vMtrlDiffuse.a)
    {
        discard;
    
    }
    
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
        
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
        
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vSpecular = vector(0.f, 0.f, 0.f, 0.f);
    if (g_HasSpecTex)
    {
        vector vSpecDesc = g_SpecTexture.Sample(LinearSampler, Input.vTex);
        vSpecDesc = vector(vSpecDesc.b, vSpecDesc.b, vSpecDesc.b, vSpecDesc.a);
    }
    
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    Output.vVelocity = Input.vDir;
    Output.vRimMask = 0.f;
    
    return Output;
}

PS_OUT PS_LerpBlur(PS_IN Input)
{
    PS_OUT Output = (PS_OUT) 0;
    
    vector vMtrlDiffuse = g_DiffuseTexture.Sample(LinearSampler, Input.vTex);
    float fDissolve = g_DissolveTexture.Sample(LinearSampler, Input.vTex).r;
    
    if (g_fDissolveRatio > fDissolve)
    {
        discard;
    }
    
    if (0.3f > vMtrlDiffuse.a)
    {
        discard;
    }
    if (g_vColor.a > 0.f)
    {
        vMtrlDiffuse = lerp(vMtrlDiffuse, g_vColor, 0.3f);
    }
    float3 vNormal;
    if (g_HasNorTex)
    {
        vector vNormalDesc = g_NormalTexture.Sample(LinearSampler, Input.vTex);
        
        vNormal = vNormalDesc.xyz * 2.f - 1.f;
        
        float3x3 WorldMatrix = float3x3(Input.vTangent, Input.vBinormal, Input.vNor.xyz);
        
        vNormal = normalize(mul(normalize(vNormal), WorldMatrix) * -1.f);
    }
    else
    {
        vNormal = normalize(Input.vNor.xyz);
    }
    
    vector vSpecular = vector(0.f, 0.f, 0.f, 0.f);
    if (g_HasSpecTex)
    {
        vector vSpecDesc = g_SpecTexture.Sample(LinearSampler, Input.vTex);
        vSpecDesc = vector(vSpecDesc.b, vSpecDesc.b, vSpecDesc.b, vSpecDesc.a);
    }
    
    
    Output.vDiffuse = vMtrlDiffuse;
    Output.vNormal = vector(vNormal.xyz * 0.5f + 0.5f, 0.f);
    Output.vDepth = vector(Input.vProjPos.z / Input.vProjPos.w, Input.vProjPos.w / g_fCamFar, 0.f, 0.f);
    Output.vSpecular = vSpecular;
    Output.vVelocity = Input.vDir;
    Output.vRimMask = 0.f;
    
    return Output;
}

technique11 DefaultTechniqueShader_VTF
{
    pass Default // 0
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main();
    }
    pass Dissolve // 1
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
    pass Motion_Blur // 2
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Motion_Blur();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Motion_Blur();
    }

 

    pass LerpDissolve // 3
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_LerpDissolve();
    }

    pass LerpBlur // 4
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Motion_Blur();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_LerpBlur();
    }

    pass Main_Rim // 5
    {
        SetRasterizerState(RS_None);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_Main();
        GeometryShader = NULL;
        HullShader = NULL;
        DomainShader = NULL;
        PixelShader = compile ps_5_0 PS_Main_Rim();
    }

    //pass Blur_Rim
    //{
    //    SetRasterizerState(RS_None);
    //    SetDepthStencilState(DSS_Default, 0);
    //    SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

    //    VertexShader = compile vs_5_0 VS_Motion_Blur();
    //    GeometryShader = NULL;
    //    HullShader = NULL;
    //    DomainShader = NULL;
    //    PixelShader = compile ps_5_0 PS_Blur_Rim();
    //}

    //pass Lerp_RimLighting
    //{
    //    SetRasterizerState(RS_None);
    //    SetDepthStencilState(DSS_Default, 0);
    //    SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

    //    VertexShader = compile vs_5_0 VS_Main();
    //    GeometryShader = NULL;
    //    HullShader = NULL;
    //    DomainShader = NULL;
    //    PixelShader = compile ps_5_0 PS_Lerp_Rim();
    //}
};