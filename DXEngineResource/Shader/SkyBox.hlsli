//	Include HLSL
#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3	PosL		: POSITION;
	float2	UV			: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float2 UV		: TEXCOORD;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0;

	float4 posV = mul(float4(vin.PosL, 0.0f), gView);
	vout.PosH = mul(posV, gProjection).xyww;

	vout.UV = vin.UV;

	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	float4 diffuse = gDiffuseTexture2D.Sample(gsamLinearWrap, pin.UV);

	return diffuse;
}