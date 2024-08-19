struct VertexIn
{
    [[vk::location(0)]] float3 Position : POSITION0;
    [[vk::location(1)]] float2 TexureCoord : TEXCOORD0;
};

struct ModelViewProjection
{
    matrix<float, 4, 4> Projection;
    matrix<float, 4, 4> View;
}; 
ConstantBuffer<ModelViewProjection> UBO : register(b0);

struct PushConstant
{
    matrix<float, 4, 4> Model;
};
[[vk::push_constant]] PushConstant PC;

struct VertexOut
{
    float4 Position : SV_Position;
    [[vk::location(0)]] float2 TexureCoord : TEXCOORD0;
};

VertexOut main(VertexIn In)
{
    VertexOut Out;
    Out.Position = mul(UBO.Projection, mul(UBO.View, mul(PC.Model, float4(In.Position.xy, -1.5f, 1.f))));
    
    Out.TexureCoord = In.TexureCoord;

    return Out;
}