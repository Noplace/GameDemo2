Texture3D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);

struct PixelShaderInput
{
	float4 pos : SV_POSITION;
  float3 uvw : TEXCOORD0;
};

#include <simplex_noise.hlsli>

SamplerState SampleType
{
    Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    ComparisonFunc  = Never;
    MinLod = 0;
    MaxLOD = FLOAT32_MAX;
};



float4 main(PixelShaderInput input) : SV_TARGET
{
  float n = 0;
  for (int octave=0; octave<4; octave++) {
     float scale = 1 / pow(2, 3-octave);
     float noise =snoise(input.uvw*10*scale);
     n += noise / pow(2, octave);
  }
  //n = snoise(input.uvw*10);
  n = (n+1.0f)*0.5f;
  n = ExpFilter(n);
  return float4(n,n,n,n);

  //float4 a = shaderTexture.Sample(SampleType, input.uvw);
  //a.a = a.x;
  //return a;
  //float n = (inoise(input.uvw/256)+0.5f)*0.5f;
	//return float4(n,n,n,0.9f);
  //return GeneratePermTexture(input.uvw.x);
  //return float4(permTexture.Sample(SampleType, input.uvw.x).x,0,0,0.9f);
}
