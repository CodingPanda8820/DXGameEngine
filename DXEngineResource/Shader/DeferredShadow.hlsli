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
	float4 ClipPos	 : POSITION;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	//	Transform to world space.
	float4 PositionW = mul(float4(vin.PositionL, 1.0f), gWorld);
	vout.PositionH = mul(mul(PositionW, gUserDataMatrix_0), gUserDataMatrix_1);
	vout.ClipPos = vout.PositionH;

	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	return float4((pin.ClipPos.z / pin.ClipPos.w), 0.0f, 0.0f, 1.0f);
};