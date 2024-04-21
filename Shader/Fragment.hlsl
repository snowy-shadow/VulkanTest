struct VertexInput
{
    float3 Color : COLOR;
};

float4 main(VertexInput In) : COLOR
{
    return float4(In.Color, 1.0);
};