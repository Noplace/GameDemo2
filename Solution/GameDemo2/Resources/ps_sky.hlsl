#include <ps_global.hlsli>

cbuffer CameraPSCB1 : register(b1)
{
	float4 cameraLookAt;
  float4 cameraPosition;
  float4 cameraUp;
	float4 cameraForward;
  float4 cameraRight;
};


static const float turbidity = 2.0;
static const float rayleighCoefficient = 2.5;

static const float mieCoefficient = 0.005;
static const float mieDirectionalG = 0.80;


// constants for atmospheric scattering
static const float e = 2.71828182845904523536028747135266249775724709369995957;
static const float pi = 3.141592653589793238462643383279502884197169;

static const float n = 1.0003; // refractive index of air
static const float N = 2.545E25; // number of molecules per unit volume for air at
						// 288.15K and 1013mb (sea level -45 celsius)

// wavelength of used primaries, according to preetham
static const float3 primaryWavelengths = float3(680E-9, 550E-9, 450E-9);

// mie stuff
// K coefficient for the primaries
static const float3 K = float3(0.686, 0.678, 0.666);
static const float v = 4.0;

// optical length at zenith for molecules
static const float rayleighZenithLength = 8.4E3;
static const float mieZenithLength = 1.25E3;
static const float3 up = float3(0.0, 0.0, 1.0);

static const float sunIntensity = 1000.0;
static const float sunAngularDiameterCos = 0.99983194915; // 66 arc seconds -> degrees, and the cosine of that

// earth shadow hack
static const float cutoffAngle = pi/1.95;
static const float steepness = 1.5;

struct Camera											// A camera struct that holds all relevant camera parameters
{
	float3 position;
	float3 lookAt;
	float3 rayDir;
	float3 forward, up, left;
};

float3 TotalRayleigh(float3 primaryWavelengths)
{
	float3 rayleigh = (8.0 * pow(pi, 3.0) * pow(pow(n, 2.0) - 1.0, 2.0)) / (3.0 * N * pow(primaryWavelengths, float3(4.0,4.0,4.0)));   // The rayleigh scattering coefficient
 
    return rayleigh; 

    //  8PI^3 * (n^2 - 1)^2 * (6 + 3pn)     8PI^3 * (n^2 - 1)^2
    // --------------------------------- = --------------------  
    //    3N * Lambda^4 * (6 - 7pn)          3N * Lambda^4         
}

float RayleighPhase(float cosViewSunAngle)
{	 
	return (3.0 / (16.0*pi)) * (1.0 + pow(cosViewSunAngle, 2.0));
}

float3 totalMie(float3 primaryWavelengths, float3 K, float T)
{
	float c = (0.2 * T ) * 10E-18;
  float p = v - 2.0;
	return 0.434 * c * pi * pow((2.0 * pi) / primaryWavelengths, float3(p,p,p)) * K;
}

float hgPhase(float cosViewSunAngle, float g)
{
	return (1.0 / (4.0*pi)) * ((1.0 - pow(g, 2.0)) / pow(1.0 - 2.0*g*cosViewSunAngle + pow(g, 2.0), 1.5));
}

float SunIntensity(float zenithAngleCos)
{
	return sunIntensity * max(0.0, 1.0 - exp(-((cutoffAngle - acos(zenithAngleCos))/steepness)));
}




float3 Uncharted2Tonemap(float3 x)
{
  float A = 0.15;
  float B = 0.50;
  float C = 0.10;
  float D = 0.20;
  float E = 0.02;
  float F = 0.30;
  float W = 1000.0;
   return ((x*(A*x+C*B)+D*E)/(x*(A*x+B)+D*F))-E/F;
}

float3 ToneMap(float3 color, float3 sunPos) {
    float3 toneMappedColor;
    
    toneMappedColor = color * 0.04;
    toneMappedColor = Uncharted2Tonemap(toneMappedColor);
 
    float sunfade = 1.0-clamp(1.0-exp(-(sunPos.z/500.0)),0.0,1.0);
    float p = 1.0/(1.2+(1.2*sunfade));
    toneMappedColor = pow(toneMappedColor,float3(p,p,p));
    
    return toneMappedColor;
}

float4 main(PixelShaderInput input) : SV_TARGET
{ 
  float3 sunDirection = normalize(float3(-(0.5)*2., -1, (0.5)*2.));
	float2 resolution = float2(1,1);
    // General parameter setup
  //input.uv.y = 1 - input.uv.y;
	float2 vPos = 2.0*input.uv/resolution - 1.0; 					// map vPos to -1..1
	//float t = totalTime*0.0;									// time value, used to animate stuff
	float screenAspectRatio = resolution.x/resolution.y;					// the aspect ratio of the screen (e.g. 16:9)
	//float3 finalColor = float3(0.1,0.1,0.1);								// The background color, dark gray in this case
	
   //Camera setup
	Camera cam;										// Make a struct holding all camera parameters
  cam.lookAt = cameraLookAt;//							// The point the camera is looking at
	cam.position = cameraPosition;//				// The position of the camera
	cam.up = cameraUp;//float3(0,0,1);									// The up vector, change to make the camera roll, in world space
  cam.forward = cameraForward;//normalize(cam.lookAt-cam.position);					// The camera forward vector, pointing directly at the lookat point
  cam.left = cameraRight; //cross(cam.forward, cam.up);							// The left vector, which is perpendicular to both forward and up
 	//cam.up = cross(cam.left, cam.forward);							// The recalculated up vector, in camera space
 
	float3 screenOrigin = (cam.position+cam.forward); 					// Position in 3d space of the center of the screen
	float3 screenHit = screenOrigin + vPos.x*cam.left*screenAspectRatio + vPos.y*cam.up; 	// Position in 3d space where the camera ray intersects the screen
  
	cam.rayDir = normalize(screenHit-cam.position);						// The direction of the current camera ray

	float3 sunPos = float3(0, -1, sin(totalTime*0.5)*0.5+0.45);
	sunDirection = normalize(sunPos);//float3(-(mouse.x-0.5)*2.*screenAspectRatio, -1, (mouse.y-0.5)*2.));	
    float3 viewDir = cam.rayDir;//normalize(wPos - cameraPos);

    // Cos Angles
    float cosViewSunAngle = dot(viewDir, sunDirection);
    float cosSunUpAngle = dot(sunDirection, up);
    float cosUpViewAngle = dot(up, viewDir);
    
    float sunE = SunIntensity(cosSunUpAngle);  // Get sun intensity based on how high in the sky it is

	// extinction (absorbtion + out scattering)
	// rayleigh coefficients
//	float3 rayleighAtX = TotalRayleigh(primaryWavelengths) * rayleighCoefficient;
    float3 rayleighAtX = float3(5.176821E-6, 1.2785348E-5, 2.8530756E-5);
    
	// mie coefficients
	float3 mieAtX = totalMie(primaryWavelengths, K, turbidity) * mieCoefficient;  
    
	// optical length
	// cutoff angle at 90 to avoid singularity in next formula.
	float zenithAngle = max(0.0, cosUpViewAngle);
    
	float rayleighOpticalLength = rayleighZenithLength / zenithAngle;
	float mieOpticalLength = mieZenithLength / zenithAngle;


	// combined extinction factor	
	float3 Fex = exp(-(rayleighAtX * rayleighOpticalLength + mieAtX * mieOpticalLength));

	// in scattering
	float3 rayleighXtoEye = rayleighAtX * RayleighPhase(cosViewSunAngle);
	float3 mieXtoEye = mieAtX *  hgPhase(cosViewSunAngle, mieDirectionalG);
     
    float3 totalLightAtX = rayleighAtX + mieAtX;
    float3 lightFromXtoEye = rayleighXtoEye + mieXtoEye; 
    
    float3 somethingElse = sunE * (lightFromXtoEye / totalLightAtX);
    
    float3 sky = somethingElse * (1.0 - Fex);
    sky *= lerp(float3(1.0f,1.0f,1.0f),pow(somethingElse * Fex,float3(0.5,0.5,0.5)),clamp(pow(1.0-dot(up, sunDirection),5.0),0.0,1.0));

    
	// composition + solar disc

    float sundisk = smoothstep(sunAngularDiameterCos,sunAngularDiameterCos+0.00002,cosViewSunAngle);
	float3 sun = (sunE * 19000.0 * Fex)*sundisk;

  float4 gl_FragColor;
  gl_FragColor.rgb = ToneMap(sky+sun,sunPos);
  gl_FragColor.a = 1.0;
  return gl_FragColor;
}