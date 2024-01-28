float3 g_fLuminace = float3(0.2126f, 0.7152f, 0.0722f);
float g_fThreshold;

struct SSAO_DESC
{
    float fIntensity;
    float fRadius;
    float fScale;
    float fBias;
};

struct HDR_DESC
{
    float fMiddleGray;
    float fLumWhiteSqr;
    float2 vPadding;
};

// Sampler
sampler PointSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = wrap;
    AddressV = wrap;
};

sampler PointClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = clamp;
    AddressV = clamp;
};

sampler PointMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Mirror;
    AddressV = Mirror;
};

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
};

sampler LinearClampSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = clamp;
    AddressV = clamp;
};

sampler LinearMirrorSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Mirror;
    AddressV = Mirror;
};

sampler WaterSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = wrap;
    AddressV = wrap;
    AddressW = wrap;
};

// Rasterizer State
RasterizerState RS_Default
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = false;
};

RasterizerState RS_CCW
{
    FillMode = Solid;
    CullMode = Back;
    FrontCounterClockWise = true;
};

RasterizerState RS_None
{
    FillMode = Solid;
    CullMode = None;
};

RasterizerState RS_Wire
{
    FillMode = Wireframe;
    CullMode = Back;
    FrontCounterClockWise = false;
};

// Depth Stencil State
DepthStencilState DSS_Default
{
    DepthEnable = true;
    DepthWriteMask = all;
    DepthFunc = Less_Equal;
};

DepthStencilState DSS_None
{
    DepthEnable = false;
    DepthWriteMask = Zero;
};

// Blend State
BlendState BS_Default
{
    BlendEnable[0] = false;
};

BlendState BS_AlphaBlend
{
    BlendEnable[0] = true;

    SrcBlend = Src_Alpha;
    DestBlend = Inv_Src_Alpha;
    BlendOp = Add;
};
BlendState BS_OnebyOne
{
    BlendEnable[0] = true;
    BlendEnable[1] = true;

    SrcBlend = One;
    DestBlend = One;
    BlendOp = Add;
};

BlendState BS_Half
{
    BlendEnable[0] = true;

    SrcBlend = Inv_Dest_Alpha;
    DestBlend = Dest_Alpha;
    BlendOp = Add;
};