#include <vs_global.hlsli>


struct VSInput
{
	float3 pos : POSITION;
  float2 uv : TEXCOORD0;
};


struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
  float2 uv : TEXCOORD0;
};

PixelShaderInput main( VSInput input ) 
{
  PixelShaderInput output;

	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;


  //output.uv = float2((id << 1) & 1,id & 2);
  //output.pos = float4(output.uv*float2(-2,-2) + float2(-1,1),0,1);
  output.color = float3(1,1,1);
  output.uv = input.uv;
  //output.pos = float4(input.pos, 1.0f);
	return output;
}