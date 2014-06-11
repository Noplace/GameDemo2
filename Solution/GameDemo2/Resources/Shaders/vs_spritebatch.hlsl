#include <vs_global.hlsli>

#define MAX_SPRITE_COUNT 1000

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 color : COLOR0;
  float2 uv : TEXCOORD0;
  uint   index : BLENDINDICES0;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR0;
  float2 uv : TEXCOORD0;
};

cbuffer SpriteBatchConstantBuffer : register(b3)
{
  struct {
    float x,y,z,scale;
    float angle;
    float2 rot_origin;
    float opacity;
  } sprite_info[MAX_SPRITE_COUNT];
};


float4 transformSprite(float4 pos, uint index) {
  float4 trans = float4(sprite_info[index].x,sprite_info[index].y,sprite_info[index].z,0);
  float4 rot_origin = float4(sprite_info[index].rot_origin.x,sprite_info[index].rot_origin.y,0,0);
  float angle = sprite_info[index].angle;
  float scale = sprite_info[index].scale;
  float4x4 scale_mat = {
    scale,0.0f,0.0f, 0,
    0.0f,scale,0.0f, 0,
    0.0f,0.0f,scale, 0,
    0.0f,0.0f,0.0f, 1
  };

  float4x4 rot_mat = {
    cos(angle),sin(angle),0,0,
    -sin(angle),cos(angle),0,0,
    0,0,1,0,
    0,0,0,1,
  };

  //based on affine transformation code from dx
  matrix tmat = scale_mat;
  tmat[3] -= rot_origin;
  tmat = mul(tmat,rot_mat);
  tmat[3] += rot_origin;
  tmat[3] += trans;
  
  return mul(pos,tmat); //column major multiplication, used to be row
}

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 pos = float4(input.pos, 1.0f);

	// Transform the vertex position into projected space.
	pos = mul(pos, model); //model is spritebatch world transform
  pos = transformSprite(pos,input.index);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.pos = pos;

	// Pass through the color without modification.
	output.color = float4(input.color,sprite_info[input.index].opacity);
  output.uv = input.uv;
	return output;
}
