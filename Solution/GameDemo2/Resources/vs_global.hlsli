cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
  matrix viewInv;
};

cbuffer TimeConstantBuffer : register(b1)
{
	float totalTime;
  float deltaTime;
};