Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);

#include <ps_global.hlsli>




/*second perlin noise*/


float CosineInterpolation( float x, float y, float fractional ) {
   float ft = 3.141592f * fractional;
   float f = ( 1.0f - cos( ft ) ) * 0.5f;

   return x * ( 1.0f - f ) + y * f;
}

float Noise(float2 xy)
{
    float2 noise = (frac(sin(dot(xy ,float2(12.9898,78.233)*2.0)) * 43758.5453));
    return abs(noise.x + noise.y) * 0.5;
}

float SmoothNoise( float integer_x, float integer_y ) {
   float corners = ( Noise( float2(integer_x - 1, integer_y - 1) ) + Noise( float2(integer_x + 1, integer_y + 1 )) + Noise( float2(integer_x + 1, integer_y - 1 )) + Noise( float2(integer_x - 1, integer_y + 1 )) ) / 16.0f;
   float sides = ( Noise( float2(integer_x, integer_y - 1 )) + Noise( float2(integer_x, integer_y + 1 )) + Noise( float2(integer_x + 1, integer_y )) + Noise( float2(integer_x - 1, integer_y )) ) / 8.0f;
   float center = Noise( float2(integer_x, integer_y )) / 4.0f;

   return corners + sides + center;
}

float InterpolatedNoise( float x, float y ) {
   float integer_x = x - frac(x), fractional_x = frac(x);
   float integer_y = y - frac(y), fractional_y = frac(y);

   float p1 = SmoothNoise( integer_x, integer_y );
   float p2 = SmoothNoise( integer_x + 1, integer_y );
   float p3 = SmoothNoise( integer_x, integer_y + 1 );
   float p4 = SmoothNoise( integer_x + 1, integer_y + 1 );

   p1 = CosineInterpolation( p1, p2, fractional_x );
   p2 = CosineInterpolation( p3, p4, fractional_x );

   return CosineInterpolation( p1, p2, fractional_y );
}

float CreatePerlinNoise( float x, float y ) {
    float result = 0.0f, amplitude = 0.0f, frequency = 0.0f, persistance = 0.1f;

    for ( int i = 1; i <= 4; i++ ) {
       frequency += 2;
       amplitude += persistance;

       result += InterpolatedNoise( x * frequency, y * frequency ) * amplitude;
    }

    return result;
}


float4 main(PixelShaderInput input) : SV_TARGET
{
  float4 textureColor;
  

    // Sample the pixel color from the texture using the sampler at this texture coordinate location.
  textureColor = shaderTexture.Sample(SampleType, input.uv);
  return textureColor*float4(1.0f,1.0f,1.0f,opacity);
  //float p = CreatePerlinNoise(input.uv.x*100,input.uv.y*100);
  //float p = inoise(float3(input.uv,0));
 // float4 color = float4(p,p,p,1.0f);
	//return color*float4(input.color,1.0f);
}
