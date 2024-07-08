struct VertexIn
{
    [[vk::location(0)]] float2 Position : POSITION0;
    [[vk::location(1)]] float3 Color : COLOR0;
};

struct ModelViewProjection
{
    float4x4 Projection;
    float4x4 View;
    float4x4 Model;
}; 

ConstantBuffer<ModelViewProjection> UBO : register(b0);

struct FragmentIn
{
    float4 Position : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR0;
};

FragmentIn main(VertexIn In)
{
    FragmentIn Out;

    Out.Position = mul(UBO.View, float4(In.Position, 0.0, 1.0));
    // Out.Position = mul(UBO.Projection, mul(UBO.View, float4(In.Position, 0.0, 1.0)));
    // Out.Position = mul(mul(float4(In.Position, 0.0, 1.0), UBO.View), UBO.Projection);
    
    // Out.Position = mul(UBO.Projection, mul(UBO.View, mul(UBO.Model, float4(In.Position, 1.0, 1.0))));
    Out.Color = In.Color;

    return Out;
};