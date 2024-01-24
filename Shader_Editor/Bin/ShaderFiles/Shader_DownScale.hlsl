//#include "Engine_Shader_Define.hlsli"

//uint g_Domain;

//uint g_GroupSize;

//struct CS_IN
//{
//    uint3 GroupID : SV_GroupID;
//    uint3 DispatchThreadID : SV_DispatchThreadID;
//    uint3 GroupThreadID : SV_GroupThreadID;
//};

//uint2 g_ScreenSize;

//Texture2D HDRTex : register(t0);
//StructuredBuffer<float> AverageValues1D : register(t1);
//RWStructuredBuffer<float> AverageLumFinal : register(u0);

//groupshared float SharedPositions[1024];

//cbuffer Adapt_Bloom_CB : register(b3)
//{
//    float g_fAdaptation;
//    float3 vEmpty;
//}

//cbuffer BloomThresholdCB : register(b4)
//{
//    float g_fBloomThreshold;
//    float3 g_vEmpty;
//}

//float DownScale4x4(uint2 CurPixel, uint GroupThreadID)
//{
//    float avgLum = 0.f;
    
//    if (CurPixel.y < g_ScreenSize.y)
//    {
//        int3 iFullResPos = int3(CurPixel * 4, 0);
//        vector vDownScaled = 0;
        
//        for (uint i = 0; i < 4; ++i)
//        {
//            for (uint j = 0; j < 4; ++j)
//            {
//                vDownScaled += HDRTex.Load(iFullResPos, int2(j, i));
//            }
//        }
        
//        vDownScaled /= 16.f;
        
//        avgLum = dot(vDownScaled.rgb, g_fLuminace);
        
//        SharedPositions[GroupThreadID] = avgLum;
        
//    }
    
//    GroupMemoryBarrierWithGroupSync();
    
//    return avgLum;
//}

//float DownScale1280to4(uint dispachThreadID, uint GroupThreadID, float avgLum)
//{
//    for (uint iGroupSize = 4, iStep1 = 1, iStep2 = 2, iStep3 = 3;
//        iGroupSize < 1280;
//        iGroupSize *= 4, iStep1 *= 4, iStep2 *= 4, iStep3 *= 4)
//    {
//        if (GroupThreadID % iGroupSize == 0)
//        {
//            float fStepAvgLum = avgLum;
            
//            fStepAvgLum += dispachThreadID + iStep1 < 1280 ? SharedPositions[GroupThreadID + iStep1] : avgLum;
            
//            fStepAvgLum += dispachThreadID + iStep2 < 1280 ? SharedPositions[GroupThreadID + iStep2] : avgLum;
            
//            fStepAvgLum += dispachThreadID + iStep3 < 1280 ? SharedPositions[GroupThreadID + iStep3] : avgLum;
            
            
//            avgLum = fStepAvgLum;
            
//            SharedPositions[GroupThreadID] = fStepAvgLum;
//        }
        
//        GroupMemoryBarrierWithGroupSync();
//    }

//    return avgLum;
//}

//void DownScale4to1(uint dispatchThreadID, uint GroupThreadID, uint GroupID, float avgLum)
//{
//    if (GroupThreadID == 0)
//    {
//        float fFinalAvgLum = avgLum;
        
//        fFinalAvgLum += dispatchThreadID + 256 < 1280 ? SharedPositions[GroupThreadID + 256] : avgLum;
        
//        fFinalAvgLum += dispatchThreadID + 512 < 1280 ? SharedPositions[GroupThreadID + 512] : avgLum;
        
//        fFinalAvgLum += dispatchThreadID + 768 < 1280 ? SharedPositions[GroupThreadID + 768] : avgLum;
        
//        fFinalAvgLum /= 1240.f;
        
//        AverageLumFinal[GroupID] = fFinalAvgLum;
//    }
//}

//[numthreads(1024, 1, 1)]
//void DownScaleFirstPass(CS_IN Input)
//{
//    uint2 vCurPixel = uint2(Input.DispatchThreadID.x % g_ScreenSize.x, Input.DispatchThreadID.x / g_ScreenSize.x);
    
//    float fAvgLum = DownScale4x4(vCurPixel, Input.GroupThreadID.x);
    
//    fAvgLum = DownScale1280to4(Input.DispatchThreadID.x, Input.GroupThreadID.x, fAvgLum);
    
//    DownScale4to1(Input.DispatchThreadID.x, Input.GroupThreadID.x, Input.GroupID.x, fAvgLum);
//}

//#define MAX_GROUP 64

//groupshared float SharedAvgFinal[MAX_GROUP];

//[numthreads(MAX_GROUP, 1, 1)]
//void DownScaleSecondPass(CS_IN Input)
//{
//    float fAvgLum = 0.f;
    
//    if (Input.DispatchThreadID.x < g_GroupSize)
//    {
//        fAvgLum = AverageValues1D[Input.DispatchThreadID.x];
//    }
    
//    SharedAvgFinal[Input.DispatchThreadID.x] = fAvgLum;
    
//    GroupMemoryBarrierWithGroupSync();
    
//    if (Input.DispatchThreadID.x % 4 == 0)
//    {
//        float fStepAvgLum = fAvgLum;
        
//        fStepAvgLum += Input.DispatchThreadID.x + 1 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 1] : fAvgLum;
        
//        fStepAvgLum += Input.DispatchThreadID.x + 2 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 2] : fAvgLum;
        
//        fStepAvgLum += Input.DispatchThreadID.x + 3 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 3] : fAvgLum;
        
//        fAvgLum = fStepAvgLum;
        
//        SharedAvgFinal[Input.DispatchThreadID.x] = fStepAvgLum;
//    }
//    GroupMemoryBarrierWithGroupSync();
    
//    if (0 == Input.DispatchThreadID.x % 16)
//    {
//        float fStepAvgLum = fAvgLum;
        
//        fStepAvgLum += Input.DispatchThreadID.x + 4 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 4] : fAvgLum;
        
//        fStepAvgLum += Input.DispatchThreadID.x + 8 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 8] : fAvgLum;
        
//        fStepAvgLum += Input.DispatchThreadID.x + 12 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 12] : fAvgLum;
        
//        fAvgLum = fStepAvgLum;
        
//        SharedAvgFinal[Input.DispatchThreadID.x] = fStepAvgLum;
//    }
    
//    GroupMemoryBarrierWithGroupSync();
    
//    if (0 == Input.DispatchThreadID.x)
//    {
//        float fFinalLumValue = fAvgLum;
        
//        fFinalLumValue += Input.DispatchThreadID.x + 16 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 16] : fAvgLum;
        
//        fFinalLumValue += Input.DispatchThreadID.x + 32 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 32] : fAvgLum;
        
//        fFinalLumValue += Input.DispatchThreadID.x + 48 < 1024 ? SharedAvgFinal[Input.DispatchThreadID.x + 48] : fAvgLum;
        
//        fFinalLumValue /= 64.f;
        
//        AverageLumFinal[0] = max(fFinalLumValue, 0.0001f);
//    }

//}

Texture2D<float4> InputTexture : register(t0);
RWTexture2D<float4> OutputTexture : register(u0);

cbuffer ThresholdParams : register(b0)
{
    float threshold;
}

[numthreads(8, 8, 1)]
void ThresholdAndDowsample(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID, 
uint GroupIndex : SV_GroupIndex, uint3 DispatchID : SV_DispatchThreadID)
{
    uint2 Pixel = uint2(DispatchID.x, DispatchID.y);
    
    uint2 inPixel = Pixel * 2;
    
    float4 hIntensity0 = lerp(InputTexture[inPixel], InputTexture[inPixel + uint2(1, 0)], 0.5f);
    float4 hIntensity1 = lerp(InputTexture[inPixel + uint2(0, 1)], InputTexture[inPixel + uint2(1, 1)], 0.5f);
    float4 Intensity = lerp(hIntensity0, hIntensity1, 0.5f);
    
    float IntensityTest = (float) (length(Intensity.rgb) > threshold);
    
    OutputTexture[Pixel] = float4(IntensityTest * Intensity.rgb, 1.f);
}