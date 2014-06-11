#include <ps_global.hlsli>

Texture2D shaderTexture : register(t0);

float4 main(PixelShaderInput input) : SV_TARGET
{
  float4 textureColor = shaderTexture.Sample(SampleType, input.uv);
  return textureColor*float4(1.0f,1.0f,1.0f,1.0f);

}