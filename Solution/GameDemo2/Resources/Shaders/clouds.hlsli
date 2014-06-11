#include <raymarch.hlsli>

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
  float3 uvw : TEXCOORD0;
  Ray eyeray : TEXCOORD1;
  float3 world_translation : TEXCOORD3;
};