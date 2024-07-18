struct VertexIn
{
    [[vk::location(0)]] float2 Position : POSITION0;
    [[vk::location(1)]] float3 Color : COLOR0;
};

struct ModelViewProjection
{
    matrix<float, 4, 4> Projection;
    matrix<float, 4, 4> View;
    matrix<float, 4, 4> Model;
}; 
ConstantBuffer<ModelViewProjection> UBO : register(b0);

struct PushConstant
{
    matrix<float, 4, 4> Model;
};
[[vk::push_constant]] PushConstant PC;

struct FragmentIn
{
    float4 Position : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR0;
};

FragmentIn main(VertexIn In)
{
    FragmentIn Out;

    Out.Position = mul(UBO.Projection, mul(UBO.View, mul(PC.Model, float4(In.Position, 1.0, 1.0))));
    Out.Color = In.Color;

    return Out;
}