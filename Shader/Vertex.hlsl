struct VertexIn
{
	[[vk::location(0)]] float2 Position : POSITION0;
    [[vk::location(1)]] float3 Color : COLOR0;
};

struct FragmentIn
{
	float4 Position : SV_POSITION;
    [[vk::location(0)]] float3 Color : COLOR0;
};

FragmentIn main(VertexIn In)
{
    FragmentIn Out;

	Out.Position = float4(In.Position, 0.0, 1.0);
	Out.Color = In.Color;

	return Out;
};