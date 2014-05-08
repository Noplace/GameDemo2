cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

cbuffer TimeConstantBuffer : register(b1)
{
	float totalTime;
  float deltaTime;
};