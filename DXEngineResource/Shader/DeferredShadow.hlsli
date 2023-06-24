// Author	: Shin yongseob(CodingPanda8820@gmail.com)
// Data		: 2023.06.11 Sunday

#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3 PositionL : POSITION;
};

struct VertexOut
{
	float4 PositionH : SV_POSITION;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//	Transform to world space.
	float4 positionW = mul(float4(vin.PositionL, 1.0f), gWorld);
	vout.PositionH = mul(positionW, gViewProjection);

	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	//return float4(pin.PositionH.z / pin.PositionH.w, 0.0f, 0.0f, 0.0f);
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
};