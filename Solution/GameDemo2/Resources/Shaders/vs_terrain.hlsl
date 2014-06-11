#include <vs_global.hlsli>
#include <simplex_noise.hlsli>

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};


float ExposureFilter(float  x,float cover = 0.6f, float sharpness = 0.98f) {



  float c = (x - (1-cover));
  if (c<0) c = 0;
  return 1 - pow(sharpness, c*255);
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 pos = float4(input.pos, 1.0f);

  float n = 0;
  for (int octave=0; octave<8; octave++) {
     float scale = 1 / pow(2, 3-octave);
     float noise = snoise(float3(pos.x,totalTime,pos.z)*0.4f*scale);
     n += noise / pow(2, octave);
  }
  n = (n+1.0f)*0.5f;
  pos.y = ExposureFilter(n,0.6f,0.98f)*3.0f;
  input.color = float4(n,0,0.2f,1.0f);
	// Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass through the color without modification.
  
	output.color = input.color;

	return output;
}
