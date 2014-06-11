//#define float min12int

Texture2D shaderTexture : register(t0);
//SamplerState SampleType : register(s0);
#include <ps_global.hlsli>


float rand(float n)
	{
	return frac(sin(n * 12.9898) * 43758.5453)-0.5;
	}



void pipe(inout float3 col, float2 p, float h)
	{
	float2 ap = abs(p);
	if (ap.y > h)
		{
		float dy = ap.y - h;
		if (dy < 60.0) ap.x *= 0.93;
		col = lerp(col, float3(0.322, 0.224, 0.290), step(ap.x, 65.0)); // outline
		if (dy > 60.0 || mod(dy, 55.0) > 5.0)
			{
			float gradient = 0.0;
			if (abs(dy - 57.5) > 7.5) gradient = max(0.0, 0.5*cos(floor((p.x+25.0)/5.0)*5.0*(0.026 - 0.006*step(dy, 10.0))));
			col = lerp(col, float3(0.322, 0.506, 0.129) + gradient, step(ap.x, 60.0)); // pipe
			}
		}
	}

// constant-array-index workaround ---
float slice(int id)
	{
	// flappy bird character (no worries, I have a tool)
	if (id == 0) return 2359296.0;
	if (id == 1) return 585.0;
	if (id == 2) return 4489216.0;
	if (id == 3) return 46674.0;
	if (id == 4) return 4751360.0;
	if (id == 5) return 2995812.0;
	if (id == 6) return 8945664.0;
	if (id == 7) return 3003172.0;
	if (id == 8) return 9469963.0;
	if (id == 9) return 7248164.0;
	if (id == 10) return 2359385.0;
	if (id == 11) return 10897481.0;
	if (id == 12) return 6554331.0;
	if (id == 13) return 9574107.0;
	if (id == 14) return 2134601.0;
	if (id == 15) return 9492189.0;
	if (id == 16) return 3894705.0;
	if (id == 17) return 9474632.0;
	if (id == 18) return 2396785.0;
	if (id == 19) return 9585152.0;
	if (id == 20) return 14380132.0;
	if (id == 21) return 8683521.0;
	if (id == 22) return 2398500.0;
	if (id == 23) return 1.0;
	if (id == 24) return 4681.0;
	return 0.0;
	}

float3 color(int id)
	{
	// flappy bird colors
	if (id == 0) return float3(0.0f,0.0f,0.0f);
	if (id == 1) return float3(0.320,0.223,0.289);
	if (id == 2) return float3(0.996,0.449,0.063);
	if (id == 3) return float3(0.965,0.996,0.965);
	if (id == 4) return float3(0.996,0.223,0.000);
	if (id == 5) return float3(0.836,0.902,0.805);
	return float3(0.965,0.707,0.191);
	}
// ---

int sprite(float2 p)
	{
	p.y +=abs(sin(totalTime*5.0)*20.0)-5.0;
	// this time it's 3 bit/px (8 colors) and 8px/slice, 204px total
	int d = 0;
	p = floor(p);
	p.x = 16.0 - p.x;
	
	if (clamp(p.x, 0.0, 16.0) == p.x && clamp(p.y, 0.0, 11.0) == p.y)
		{
		float k = p.x + 17.0*p.y;
		float s = floor(k / 8.0);
		float n = slice(int(s));
		k = (k - s*8.0)*3.0;
		if (int(mod(n/(pow(2.0,k)),2.0)) == 1) 		d += 1;
		if (int(mod(n/(pow(2.0,k+1.0)),2.0)) == 1) 	d += 2;
		if (int(mod(n/(pow(2.0,k+2.0)),2.0)) == 1) 	d += 4;
		}
	return d;
	}

void hero(inout float3 col, float2 p, float angle)
	{
	p = float2(p.x * cos(angle) - p.y * sin(angle), p.y * cos(angle) + p.x * sin(angle));
	int i = sprite(p*0.2);
	col = lerp(col, color(i), min(1.0, float(i)));
	}

void ground(inout float3 col, float2 p)
	{
	p = floor(p);
	if (p.y > -280.0) return;
	if (p.y < -285.0) col = color(1);
	if (p.y < -290.0) col = float3(0.902, 1.000, 0.549);
	if (p.y < -295.0) col = lerp(float3(0.612, 0.906, 0.353), float3(0.451, 0.745, 0.192), step(mod(p.x-floor(p.y/5.0)*5.0, 60.0), 30.0));
	if (p.y < -325.0) col = float3(0.322, 0.506, 0.129);
	if (p.y < -330.0) col = float3(0.839, 0.667, 0.290);
	if (p.y < -335.0) col = float3(0.871, 0.843, 0.580);
	}

void sky(inout float3 col, float2 p)
	{
	col = lerp(col, float3(1.0f,1.0f,1.0f), 0.3*sin(p.y*0.01));
	}

float hAt(float i)
	{
	return 250.0*rand(i*1.232157);
	}



float4 main(PixelShaderInput input) : SV_TARGET
{
  //float4 textureColor;
  //textureColor = shaderTexture.Sample(SampleType, input.uv);
  //return textureColor*float4(input.color*sin(totalTime),1.0f);
  input.uv.y = 1 - input.uv.y;
	float s = 2000.0;
	float2 p = max(1.6666667, s)*(input.uv - 0.5f);
	float dx = totalTime * 320.0;
	p.x += dx;
	
	float3 col = float3(0.322, 0.745, 0.808);
	sky(col, float2(0.0, -100.0)-p);
	
	pipe(col, float2(mod(p.x, 400.0)-200.0, p.y + hAt(floor(p.x / 400.0)) - 80.0), 110.0);
	
	float hx = dx - 200.0; // hero x
	float sx = hx - 300.0; // sample x
	float i = floor(sx/400.0); // instance
	float ch = hAt(i); // current height
	float nh = hAt(i+1.0); // next height
	float bh = 0.;// abs(60.0*sin(iChannelTime[0]*6.0)); // bounce height
	float hy = bh - lerp(ch, nh, min(1.0, mod(sx, 400.0)*0.005)) + 80.0; // hero y
	float angle = -min(0.1, 0.002*(bh)); // should be optimized
	hero(col, float2(hx, hy)-p, angle);
	
	ground(col, p);
	return float4(col,1.0);
}
