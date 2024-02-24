struct Vertex_Instancing
{
    vector vRight;
    vector vUp;
    vector vLook;
    vector vPos;
    vector vPrevPos;
    float fIndex;
    float fDissolveRatio;

    float fSpeed;
    float2 vLifeTime;
    vector vOriginPos;
    vector vDirection;
    vector vOriginDir;
};

StructuredBuffer<Vertex_Instancing> InputInfo : register(t0);
RWStructuredBuffer<Vertex_Instancing> OutputInfo : register(u0);

cbuffer ParticleParams : register(b0)
{
    uint iNumInstances;
    uint iNumUse;
    float fTimeDelta;
    float fAppearRatio;
    
    vector vGravityDir;
    
    matrix WorldMatrix;
    
    float fDissolveRatio;
    int isLoop;
    int bApplyGravity;
    int isFirstUpdate;
    
    float4 Padding;
}

[numthreads(1024, 1, 1)]
void particle(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    if (dispatchID.x < iNumInstances)
    {
        Vertex_Instancing pVertex = InputInfo[dispatchID.x];
        
        if (pVertex.vLifeTime.x == 0)
        {
            pVertex.vPos = mul(pVertex.vOriginPos, WorldMatrix);
            pVertex.vPrevPos = pVertex.vPos;

            matrix WorldRotation = WorldMatrix;
            WorldRotation._11_12_13_14 = normalize(WorldRotation._11_12_13_14);
            WorldRotation._21_22_23_24 = normalize(WorldRotation._21_22_23_24);
            WorldRotation._31_32_33_34 = normalize(WorldRotation._31_32_33_34);
            WorldRotation._41_42_43_44 = vector(0.f, 0.f, 0.f, 1.f);
            pVertex.vDirection = mul(pVertex.vOriginDir, WorldRotation);
        }

        if (isLoop && isFirstUpdate)
        {
            pVertex.vPos = vector(0.f, -30000.f, 0.f, 1.f);
            pVertex.vPrevPos = pVertex.vPos;
        }

        if (bApplyGravity)
        {
            float fAlpha = (pVertex.vLifeTime.x / pVertex.vLifeTime.y) * 0.7f;
            pVertex.vDirection = lerp(pVertex.vDirection, vGravityDir, fAlpha);
        }

        if (fAppearRatio > 0.f && pVertex.vLifeTime.x <= pVertex.vLifeTime.y * fAppearRatio)
        {
            pVertex.fDissolveRatio = 1.f - (pVertex.vLifeTime.x / (pVertex.vLifeTime.y * fAppearRatio));
            pVertex.fDissolveRatio = min(pVertex.fDissolveRatio, 1.f);
        }
        else if (pVertex.vLifeTime.x > pVertex.vLifeTime.y * fDissolveRatio)
        {
            pVertex.fDissolveRatio = (pVertex.vLifeTime.x - (pVertex.vLifeTime.y * fDissolveRatio)) / (pVertex.vLifeTime.y * (1.f - fDissolveRatio));
            pVertex.fDissolveRatio = min(pVertex.fDissolveRatio, 1.f);
        }
        else
        {
            pVertex.fDissolveRatio = 0.f;
        }
        
        pVertex.fIndex = saturate(pVertex.vLifeTime.x / pVertex.vLifeTime.y);

        pVertex.vPrevPos = pVertex.vPos;
        pVertex.vPos += pVertex.vDirection * pVertex.fSpeed * fTimeDelta;
        pVertex.vLifeTime.x += fTimeDelta;

        if (dispatchID.x >= iNumUse)
        {
            pVertex.vPos = vector(-1000.f, -1000.f, -1000.f, 1.f);
        }
        else if (pVertex.vLifeTime.x > pVertex.vLifeTime.y)
        {
            if (isLoop == 1)
            {
                pVertex.vLifeTime.x = 0.f;
                pVertex.vPos = mul(vector(pVertex.vOriginPos.xyz, 1.f), WorldMatrix);
                pVertex.vPrevPos = pVertex.vPos;
            }
            else
            {
                pVertex.vPos = vector(0.f, -10000.f, 0.f, 1.f);
                pVertex.vPrevPos = pVertex.vPos;
            }
        }

        OutputInfo[dispatchID.x] = pVertex;
    }
}