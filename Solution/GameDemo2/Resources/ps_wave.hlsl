Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer LoadingScenePSCB : register(b0)
{
	float totalTime;
  float deltaTime;
};


struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
  float2 uv : TEXCOORD0;
};

float3 beam(float2 position) {
	float beam = 1.0 - abs(position.y - 0.5) * 2.0;
	beam = smoothstep(0.7, 1.0, beam);
	return float3(0.25, 0.75, 1.0) * beam;
}

float3 wave(float2 position, float frequency, float height, float speed) {
	float sinVal = sin(position.x * frequency - totalTime * speed) * height;
	sinVal = sinVal * 0.5 + 0.5;
	
	float wave = 1.0 - abs(sinVal - position.y);
	wave = smoothstep(0.95, 1.0, wave);
	
	return float3(wave,wave,wave);
}

float4 main(PixelShaderInput input) : SV_TARGET
{
  float4 textureColor;
  textureColor = shaderTexture.Sample(SampleType, input.uv);
  //return textureColor*float4(input.color*sin(totalTime),1.0f);

  float3 result = 0;
	
	result += beam(input.uv);
	result += wave(input.uv, 20.0, 0.10,  4.0);
	result += wave(input.uv, 11.0, 0.25,  1.0);
	result += wave(input.uv, 50.0, 0.02, 25.0);
	//result.x = textureColor.x;
	return float4(result, 1.0);
}
