//	Include HLSL
#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3	PosL		: POSITION;
	float3	NormalL		: NORMAL;
	float2	UV			: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float2 UV		: TEXCOORD;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;
	vout.PosH = mul(posW, gViewProjection);

	vout.UV.x = vin.UV.x;
	vout.UV.y = vin.UV.y;

	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	float4 diffuse = gDiffuse;
	if (gDiffuseTextureOn)
		diffuse = gDiffuseTexture2D.Sample(gsamLinearWrap, pin.UV) * gDiffuse;

	return diffuse;
}