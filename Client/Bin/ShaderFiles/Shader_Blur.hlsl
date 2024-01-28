#define GAUSSIAN_RADIUS 7

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

cbuffer BlurParams : register(b0)
{
    // = float coefficients[GAUSSIAN_RADIUS + 1]
    // radius <= GAUSSIAN_RADIUS, direction 0 = horizontal, 1 = vertical
    int2 radiusAndDirection;
    uint2 iWinSize;
}

[numthreads(8, 8, 1)]
void Blur(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint groupIndex : SV_GroupIndex, uint3 dispatchID : SV_DispatchThreadID)
{
    int2 pixel = int2(dispatchID.x, dispatchID.y);

    if (pixel.x < iWinSize.x && pixel.y < iWinSize.y)
    {
        int radius = radiusAndDirection.x;
        int2 dir = int2(1 - radiusAndDirection.y, radiusAndDirection.y);

        float4 accumulatedValue = float4(0.0, 0.0, 0.0, 0.0);
        
        float Gaussian[15] =
        {
            0.000560f,
            0.002216f,
            0.008764f,
            0.026995f,
            0.064759f,
            0.120985f,
            0.176033f,
            0.199471f,
            0.176033f,
            0.120985f,
            0.064759f,
            0.026995f,
            0.008764f,
            0.002216f,
            0.000560f
        };
      
        //float Gaussian[15] = { 0.000067f, 0.000437f, 0.002216f, 0.008765f, 0.027001f, 0.064764f, 0.121012f, 0.199500f, 0.121012f, 0.064764f, 0.027001f, 0.008765f, 0.002216f, 0.000437f, 0.000067f };
        for (int i = -radius; i <= radius; ++i)
        {
            uint cIndex = (uint) (i + radius);
            //accumulatedValue += coefficients[cIndex >> 2][cIndex & 3] * inputTexture[mad(i, dir, pixel)];
            accumulatedValue += Gaussian[cIndex] * inputTexture[mad(i, dir, pixel)] * 2.f;
        }
        
        outputTexture[pixel] = accumulatedValue;
    }
}