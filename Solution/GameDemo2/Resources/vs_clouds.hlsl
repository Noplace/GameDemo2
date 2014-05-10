#include <vs_global.hlsli>
#include <clouds.hlsli>

struct VertexShaderInput
{
	float3 pos : POSITION;
  float3 uvw : TEXCOORD0;
};



static const float foclen = 2500.0f;

void RayMarchVS(inout float3 pos : POSITION,
				in float4 texcoord : TEXCOORD0,
				out Ray eyeray : TEXCOORD1
				)
{
	// calculate world space eye ray
	// origin
	eyeray.o = mul(float4(0, 0, 0, 1), viewInv);
  float2 viewport = {640,480};
	// direction
	eyeray.d.xy = ((texcoord.xy*2.0)-1.0) * viewport;
	eyeray.d.y = -eyeray.d.y;	// flip y axis
	eyeray.d.z = foclen;
	
	eyeray.d = mul(eyeray.d, (float3x3) viewInv);
}


VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
 
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;
  RayMarchVS(input.pos,pos,output.eyeray);
  output.uvw = input.uvw;
	return output;
}
