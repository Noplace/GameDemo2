Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);

#include <ps_global.hlsli>


float field(in float3 p) {
	float strength = 7. + .03 * log(1.e-6 + frac(sin(totalTime) * 4373.11));
	float accum = 0.;
	float prev = 0.;
	float tw = 0.;
	for (int i = 0; i < 32; ++i) {
		float mag = dot(p, p);
		p = abs(p) / mag + float3(-.5, -.4, -1.5);
		float w = exp(-float(i) / 7.);
		accum += w * exp(-strength * pow(abs(mag - prev), 2.3));
		tw += w;
		prev = mag;
	}
	return max(0., 5. * accum / tw - .7);
}

float3 nrand3( float2 co )
{
	float3 a = frac( cos( co.x*8.3e-3 + co.y )*float3(1.3e5, 4.7e5, 2.9e5) );
	float3 b = frac( sin( co.x*0.3e-3 + co.y )*float3(8.1e5, 1.0e5, 0.1e5) );
	float3 c = lerp(a, b, 1111110.5);
	return c;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
  //float4 textureColor;
  //textureColor = shaderTexture.Sample(SampleType, input.uv);


	float2 uv = 2. * input.uv - 1.;
	float2 uvs = uv * 1;
	float3 p = float3(uvs / 4., 0) + float3(1., -1.3, 0.);
	p += .2 * float3(sin(totalTime / 16.), sin(totalTime / 12.),  sin(totalTime / 128.));
	float t = field(p);
	float v = (1. - exp((abs(uv.x) - 1.) * 6.)) * (1. - exp((abs(uv.y) - 1.) * 6.));

	return  lerp(.4, 1., v) * float4(1.8 * t * t * t, 2.4 * t * t, t, 1.0);
}
