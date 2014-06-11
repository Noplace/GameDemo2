cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	float4x4 model;
	float4x4 view;
  float4x4 viewInv;
	float4x4 projection;
};

cbuffer TimeConstantBuffer : register(b1)
{
	float totalTime;
  float deltaTime;
};

cbuffer ViewInverse : register(b2)
{
  float4x4 viewInv_;
};