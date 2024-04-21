struct VertexIn
{
	float2 Position : POSITION;
	float3 Color : COLOR;
};

struct FragmentIn
{
	float4 Position : SV_POSITION;
	float3 Color : COLOR;
};

FragmentIn main(VertexIn In)
{
	FragmentIn Out;

	Out.Position = float4(In.Position, 0.0, 1.0);
	Out.Color = In.Color;

	return Out;
};