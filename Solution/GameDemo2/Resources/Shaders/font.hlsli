
Texture2DArray textures : register( t0 );

SamplerState samLinear
{
Filter = MIN_MAG_MIP_LINEAR;
AddressU = Clamp;
AddressV = Clamp;
AddressW = Wrap;
ComparisonFunc = NEVER;
MinLOD = 0;
//MaxLOD = MAX;
};


cbuffer cbMisc : register ( b1 )
{
	matrix worldTransform;
	float globalAlpha;
	float4 globalColor;
};

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
  float3 Pos : POSITION;
  float4  color : COLOR0;
  float2 Tex : TEXCOORD0;
	uint Channel : BLENDINDICES0;
	uint Page : BLENDINDICES1;
};

struct PS_INPUT
{
  float4 Pos : SV_POSITION;
  float2 Tex : TEXCOORD0;
  float4  color : COLOR0;
  uint Channel : BLENDINDICES0;
  uint Page : BLENDINDICES1;
};
