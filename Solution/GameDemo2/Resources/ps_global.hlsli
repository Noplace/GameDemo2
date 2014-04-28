struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
  float2 uv : TEXCOORD0;
};

cbuffer PSCB0 : register(b0)
{
	float totalTime;
  float deltaTime;
  float opacity;
};


SamplerState SampleType
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    ComparisonFunc  = Never;
    MinLod = 0;
    MaxLOD = FLOAT32_MAX;
};

float mod(float x, float y) {
  return x - y * floor(x/y);
}