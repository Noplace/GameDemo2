#include <clouds.hlsli>
#include <simplex_noise.hlsli>


Texture3D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);

Texture3D volumeTexture
<
    string texturetype = "3D";
	string function = "GenerateVolumeNoise";
//	int width = 128, height = 128, depth = 128;
	int width = 64, height = 64, depth = 64;
>;

SamplerState volumeSampler
{
	Texture = <volumeTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

float4 GenerateVolumeNoise(float3 p : POSITION) : COLOR
{
return float4(1,1,1,1);
	/*const float noisescale = 5.0;
	p *= noisescale;
	return float4(noise(p),
				  noise(p + float3(5, 7, 13)),
				  noise(p + float3(9, 37, 57)),
				  noise(p + float3(17, 21, 99)));*/
}

SamplerState SampleType
{
    Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
    AddressW = Wrap;
    ComparisonFunc  = Never;
    MinLod = 0;
    MaxLOD = FLOAT32_MAX;
};

float4 volume(float3 P) {
  float n = 0;
  for (int octave=0; octave<4; octave++) {
     float scale = 1 / pow(2, 3-octave);
     float noise =snoise(P*10*scale);
     n += noise / pow(2, octave);
  }
  //n = snoise(input.uvw*10);
  n = (n+1.0f)*0.5f;
  n = ExpFilter(n);
  return float4(n,n,n,n);
}

static const float brightness = 25.0f;
static const float3 boxMin = { -1.0, -1.0, -1.0 };
static const float3 boxMax = { 1.0, 1.0, 1.0 };

// Pixel shader
float4 RayMarchPS(Ray eyeray : TEXCOORD0, uniform int steps=30) : SV_TARGET
{
	eyeray.d = normalize(eyeray.d);

	// calculate ray intersection with bounding box
	float tnear, tfar;
	bool hit = IntersectBox(eyeray, boxMin, boxMax, tnear, tfar);
	if (!hit) discard;

	// calculate intersection points
	float3 Pnear = eyeray.o + eyeray.d*tnear;
	float3 Pfar = eyeray.o + eyeray.d*tfar;
		
	// map box world coords to texture coords
	Pnear = Pnear*0.5 + 0.5;
	Pfar = Pfar*0.5 + 0.5;
	
	// march along ray, accumulating color
	float4 c = 0;
	float3 Pstep = (Pnear - Pfar) / (steps-1);
	float3 P = Pfar;
	// this compiles to a real loop in PS3.0:
	for(int i=0; i<steps; i++) {		
		float4 s = volume(P);
		c = (1.0-s.a)*c + s.a*s;
		P += Pstep;
	}
	c /= steps;
	c *= brightness;

//	return hit;
//	return tfar - tnear;
	return c;
}

float4 main(VertexShaderOutput input) : SV_TARGET
{

//return volumeTexture.Sample(volumeSampler,input.uvw);
  //return volume(input.uvw);

  return RayMarchPS(input.eyeray);
  //float4 a = shaderTexture.Sample(SampleType, input.uvw);
  //a.a = a.x;
  //return a;
  //float n = (inoise(input.uvw/256)+0.5f)*0.5f;
	//return float4(n,n,n,0.9f);
  //return GeneratePermTexture(input.uvw.x);
  //return float4(permTexture.Sample(SampleType, input.uvw.x).x,0,0,0.9f);
}
