Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);

#include <ps_global.hlsli>




/*Perlin Noise*/
static int permutation[] = {

208, 83,140,107, 57, 30,202, 48,114, 61, 66, 71,157,142, 41,100,
113,133,218,109, 95,142,189, 63,234,226,116,124, 67,247,234, 98,
140,199,243, 92,214, 87,252, 91, 93,140,218,208,187, 90, 42,105,
196, 38,136, 80,164,171,142,234,179,219, 41, 54, 85,142, 14, 62,
 45,152,143,138,126, 23,215,227,203,186,239, 20,201, 91, 72,249,
202,  8, 83,236,187, 28,116,231,207,235, 61, 95,218,144,230,177,
198, 39, 61,  0, 91,216, 38,240,124, 86, 83,155,163,200,195, 68,
161,166,122,  8,121, 43,247, 65, 50, 13, 64,220,246,229,214, 92,
183,103,224, 32, 19, 51,148, 76,142, 73, 47,  7, 65,127, 49,117,
  5,124, 61, 21,118,138, 40,182, 82,  0,224,220,  2, 36, 91,192,
 95,233, 51,201, 19,242, 39,168,222,131,  1, 74, 54, 82,230,101,
222,186,119,120, 61,108, 73,211,124,195,159,244,215, 66,201,205,
 66, 35,202, 11,231, 67,205,  6,164,  9,238,205,121,176, 70, 25,
238,209,231, 88, 58,124,109, 77,226, 99, 93,133,  5,225, 28, 69,
250,130, 55,  5,171,190,144,169,255,204,174, 81,199,211,254, 68,
186,187,173, 12,  5,  8,117, 99,180,  3,  2,130,110, 80,237, 34,

};

Texture1D permTexture 
<

  string texturetype = "1D";

  string format = "l8";

  string function = "GeneratePermTexture";

  int width = 256, height = 1;
>;

float4 GeneratePermTexture(float p : POSITION) : COLOR

{

  return permutation[p * 256] / 255.0;

}

sampler permSampler = sampler_state

{

  texture = <permTexture>;

  AddressU  = Wrap;

  AddressV  = Clamp;

  MAGFILTER = POINT;

  MINFILTER = POINT;

  MIPFILTER = NONE;

};

// gradients for 3D noise

static float3 g[] = {

  1,1,0,    -1,1,0,    1,-1,0,    -1,-1,0,

  1,0,1,    -1,0,1,    1,0,-1,    -1,0,-1,

  0,1,1,    0,-1,1,    0,1,-1,    0,-1,-1,

  1,1,0,    0,-1,1,    -1,1,0,    0,-1,-1,

};



Texture2D gradTexture

<

  string texturetype = "2D";

  string format = "q8w8v8u8";

  string function = "GenerateGradTexture";

  int width = 16, height = 1;



>;



float3 GenerateGradTexture(float p : POSITION) : COLOR

{

  return g[p * 16];

}



sampler gradSampler = sampler_state

{

  texture = <gradTexture>;

  AddressU  = Wrap;

  AddressV  = Clamp;

  MAGFILTER = POINT;

  MINFILTER = POINT;

  MIPFILTER = NONE;

};

float3 fade(float3 t)

{

  return t * t * t * (t * (t * 6 - 15) + 10); // new curve

//  return t * t * (3 - 2 * t); // old curve

}



float perm(float x)

{

  //return tex1D(permSampler, x / 256.0) * 256;
  return permTexture.Sample(permSampler, x / 256.0) * 256;
}



float grad(float x, float3 p)

{

  return dot(gradTexture.Sample(gradSampler,x) , p);

}



// 3D version

float inoise(float3 p)

{

  float3 P = fmod(floor(p), 256.0);

  p -= floor(p);

  float3 f = fade(p);



  // HASH COORDINATES FOR 6 OF THE 8 CUBE CORNERS



  float A = perm(P.x) + P.y;

  float AA = perm(A) + P.z;

  float AB = perm(A + 1) + P.z;

  float B =  perm(P.x + 1) + P.y;

  float BA = perm(B) + P.z;

  float BB = perm(B + 1) + P.z;



  // AND ADD BLENDED RESULTS FROM 8 CORNERS OF CUBE



  return lerp(

    lerp(lerp(grad(perm(AA), p),

              grad(perm(BA), p + float3(-1, 0, 0)), f.x),

         lerp(grad(perm(AB), p + float3(0, -1, 0)),

              grad(perm(BB), p + float3(-1, -1, 0)), f.x), f.y),

    lerp(lerp(grad(perm(AA + 1), p + float3(0, 0, -1)),

              grad(perm(BA + 1), p + float3(-1, 0, -1)), f.x),

         lerp(grad(perm(AB + 1), p + float3(0, -1, -1)),

              grad(perm(BB + 1), p + float3(-1, -1, -1)), f.x), f.y),

    f.z);

}

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
  return textureColor * float4(1.0f,1.0f,1.0f,opacity);
  //float p = CreatePerlinNoise(input.uv.x*100,input.uv.y*100);
  //float p = inoise(float3(input.uv,0));
 // float4 color = float4(p,p,p,1.0f);
	//return color*float4(input.color,1.0f);
}
