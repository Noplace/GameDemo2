#include <font.hlsli>
//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 main( PS_INPUT input) : SV_Target
{

    float4 pixel = 0;
	uint index = 0;
	pixel = textures.Sample( samLinear, float3(input.Tex,input.Page) );


	/*
    // Are we rendering a colored image, or 
    // a character from only one of the channels
    if( dot(vector(1,1,1,1), input.Channel) )
    {
        // Get the pixel value
		float val = dot(pixel, input.Channel);
		
        pixel.rgb = 1;
        pixel.a   = val;
    }
	*/

	//pixel.rgb = globalColor.rgb;
	//pixel.a = pixel.a * globalAlpha;
	return pixel*input.color;
}
