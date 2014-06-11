#include <ps_global.hlsli>

cbuffer CameraPSCB1 : register(b1)
{
	float4 cameraLookAt;
  float4 cameraPosition;
  float4 cameraUp;
	float4 cameraForward;
  float4 cameraRight;
};

// Created by inigo quilez - iq/2013
// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.

//#define FULL_PROCEDURAL

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

float4 main(PixelShaderInput input) : SV_TARGET
{
float2 iMouse = {0,0};
  float2 iResolution = {1,1};
	  float2 q = input.uv.xy / iResolution.xy;
    float2 p = -1.0 + 2.0*q;
    p.x *= iResolution.x/ iResolution.y;
    float2 mo = -1.0 + 2.0*iMouse.xy / iResolution.xy;
    


    // camera
    float3 ro =  4.0*normalize(float3(cos(2.75-3.0*mo.x), 0.7+(mo.y+1.0), sin(2.75-3.0*mo.x)));
	float3 ta = float3(0.0, 1.0, 0.0);
    float3 ww = normalize( ta - ro);
    float3 uu = normalize(cross( cameraLookAt, ww ));
    float3 vv = normalize(cross(ww,uu));
    float3 rd = normalize( p.x*uu + p.y*vv + 1.5*ww );

	
    float4 res = raymarch( ro, rd );

	float sun = clamp( dot(sundir,rd), 0.0, 1.0 );
	float3 col = float3(0.6,0.71,0.75) - rd.y*0.2*float3(1.0,0.5,1.0) + 0.15*0.5;
	col += 0.2*float3(1.0,.6,0.1)*pow( sun, 8.0 );
	col *= 0.95;
	col = lerp( col, res.xyz, res.w );
	col += 0.1*float3(1.0,0.4,0.2)*pow( sun, 3.0 );
	    
  return float4( col, 1.0 );
}

