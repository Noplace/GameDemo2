#include <vs_global.hlsli>
#include <font.hlsli>
//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT main( VS_INPUT input )
{
  PS_INPUT output = (PS_INPUT)0;
	float4 pos = float4(input.Pos,1);
  pos = mul(  pos, model );
  pos = mul( pos, view );
  output.Pos = mul( pos, projection );
  output.Tex = input.Tex;
	output.color = input.color; 
	output.Channel = input.Channel;
	output.Page = input.Page;
  return output;
}