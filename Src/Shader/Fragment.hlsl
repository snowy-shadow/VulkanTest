cbuffer DiffuseColor : register(b0, space1)
{
    float4 DiffuseColor;
}

Texture2D Texture0 : register(t1, space1);
SamplerState Sampler0 : register(s1, space1);

float4 main([[vk::location(0)]] float2 TextureCoord : TEXCOORD0) : SV_Target
{
    return Texture0.Sample(Sampler0, TextureCoord) * DiffuseColor;
    
    // return DiffuseColor;
}