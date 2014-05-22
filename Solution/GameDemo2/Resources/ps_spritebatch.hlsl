Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);


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

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
  float2 uv : TEXCOORD0;
};


float4 main(VertexShaderOutput input) : SV_TARGET
{
  float4 textureColor;
  textureColor = shaderTexture.Sample(SampleType, input.uv);
  
  return textureColor*input.color;//float4(1.0f,1.0f,1.0f,input.color.w);
}
