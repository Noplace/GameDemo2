#include <clouds.hlsli>
#include <simplex_noise.hlsli>

cbuffer PSCB0 : register(b0)
{
	float totalTime;
  float deltaTime;
};

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


float ExpFilter(float  x) {

  float cover = 0.5f;
  float sharpness = 0.98f;

  float c = (x - (1-cover));
  if (c<0) c = 0;
  return 1 - pow(sharpness, c*255);
}




float4 volume(float3 P) {
 P.z += totalTime;
  float n = 0;
  /*for (int octave=0; octave<4; octave++) {
     float scale = 1 / pow(2, 3-octave);
     float noise =snoise(P*10*scale);
     n += noise / pow(2, octave);
  }*/
  n += snoise(P*10*(1.0f/8.0f)) / 1.0f;
  n += snoise(P*10*(1.0f/4.0f)) / 2.0f;
  n += snoise(P*10*(1.0f/2.0f)) / 4.0f;
  n += snoise(P*10*(1.0f/1.0f)) / 8.0f;
  //n = snoise(input.uvw*10);
  n = (n+1.0f)*0.5f;
  //n = ExpFilter(n);
  return float4(n,n,n,n);
}

static const float brightness = 50.0f;
static const float3 boxMin = { -10.0, -1.0, -10.0 };
static const float3 boxMax = { 10.0, 1.0, 10.0 };

//static const float3 boxMin = { -1.0, -1.0, -1.0 };
//static const float3 boxMax = { 1.0, 1.0, 1.0 };
// Pixel shader
float4 RayMarchPS(VertexShaderOutput input, uniform int steps=30) : SV_TARGET
{
	input.eyeray.d = normalize(input.eyeray.d);
  float3 t_boxMin = boxMin + input.world_translation;
  float3 t_boxMax = boxMax + input.world_translation;
	// calculate ray intersection with bounding box
	float tnear, tfar;
	bool hit = IntersectBox(input.eyeray, t_boxMin, t_boxMax, tnear, tfar);
	if (!hit) discard;

	// calculate intersection points
	float3 Pnear = input.eyeray.o + input.eyeray.d*tnear;
	float3 Pfar = input.eyeray.o + input.eyeray.d*tfar;
		
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


// Pixel shader
float4 RayMarchPS2(VertexShaderOutput input, uniform int steps=30) : SV_TARGET
{
	input.eyeray.d = normalize(input.eyeray.d);

  float stepSize = 0.2f;
  float3 samplingPoint = input.uvw + normalize(input.eyeray.d)*stepSize;
	// calculate ray intersection with bounding box
	float tnear, tfar;
	bool hit = IntersectBox(input.eyeray, boxMin, boxMax, tnear, tfar);
	if (!hit) discard;

  float4 color = 0;
  for(int i=0; i<steps; i++) {		
    float v = volume(samplingPoint);
    samplingPoint += normalize(input.eyeray.d)*stepSize;
    color += v;
  }
  color /= steps;

	return color;
}




// hash based 3d value noise
float hash( float n )
{
    return frac(sin(n)*43758.5453);
}
float noise( in float3 x )
{
    float3 p = floor(x);
    float3 f = frac(x);

    f = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
    return lerp(lerp(lerp( hash(n+  0.0), hash(n+  1.0),f.x),
                   lerp( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
               lerp(lerp( hash(n+113.0), hash(n+114.0),f.x),
                   lerp( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
}


float4 map( in float3 p )
{
	float d = 0.2 - p.y;

	float3 q = p - float3(1.0,0.1,0.0)*totalTime;
	float f;
    f  = 0.5000*noise( q ); q = q*2.02;
    f += 0.2500*noise( q ); q = q*2.03;
    f += 0.1250*noise( q ); q = q*2.01;
    f += 0.0625*noise( q );

	d += 3.0 * f;

	d = clamp( d, 0.0, 1.0 );
	
	float4 res = float4( d,d,d,d );

	res.xyz = lerp( 1.15*float3(1.0,0.95,0.8), float3(0.7,0.7,0.7), res.x );
	
	return res;
}


static const float3 sundir = float3(-1.0,0.0,0.0);


float4 raymarch( in float3 ro, in float3 rd )
{
	float4 sum = float4(0, 0, 0, 0);

	float t = 0.0;
	for(int i=0; i<64; i++)
	{
		if( sum.a > 0.99 ) continue;

		float3 pos = ro + t*rd;
		float4 col = map( pos );
		
		#if 1
		float dif =  clamp((col.w - map(pos+0.3*sundir).w)/0.6, 0.0, 1.0 );

        float3 lin = float3(0.65,0.68,0.7)*1.35 + 0.45*float3(0.7, 0.5, 0.3)*dif;
		col.xyz *= lin;
		#endif
		
		col.a *= 0.35;
		col.rgb *= col.a;

		sum = sum + col*(1.0 - sum.a);	

        #if 0
		t += 0.1;
		#else
		t += max(0.1,0.025*t);
		#endif
	}

	sum.xyz /= (0.001+sum.w);

	return clamp( sum, 0.0, 1.0 );
}



float4 main(VertexShaderOutput input) : SV_TARGET
{

/*  float4 res = raymarch( input.eyeray.o, input.eyeray.d );

	float sun = clamp( dot(sundir,input.eyeray.d), 0.0, 1.0 );
	float3 col = float3(0.6,0.71,0.75) - input.eyeray.d.y*0.2*float3(1.0,0.5,1.0) + 0.15*0.5;
	col += 0.2*float3(1.0,.6,0.1)*pow( sun, 8.0 );
	col *= 0.95;
	col = lerp( col, res.xyz, res.w );
	col += 0.1*float3(1.0,0.4,0.2)*pow( sun, 3.0 );
  return float4(col,1);*/

//return volumeTexture.Sample(volumeSampler,input.uvw);
  //return volume(input.uvw);


  

  return RayMarchPS(input);
  

//float4 a = shaderTexture.Sample(SampleType, input.uvw);
  //a.a = a.x;
  //return a;
  //float n = (inoise(input.uvw/256)+0.5f)*0.5f;
	//return float4(n,n,n,0.9f);
  //return GeneratePermTexture(input.uvw.x);
  //return float4(permTexture.Sample(SampleType, input.uvw.x).x,0,0,0.9f);
}
