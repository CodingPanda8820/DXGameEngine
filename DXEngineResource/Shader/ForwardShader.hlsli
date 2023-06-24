#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

//	Include HLSL
#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3	PosL		: POSITION;
	float3	NormalL		: NORMAL;
	float3	Tangent		: TANGENT;
	float2	UV			: TEXCOORD;
};

struct VertexOut
{
	float4 PosH		: SV_POSITION;
	float3 PosW		: POSITION;
	float3 NormalW	: NORMAL;
	float3 TangentW : TANGENT;
	float2 UV		: TEXCOORD;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0.0f;

	float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
	vout.PosW = posW.xyz;
	vout.PosH = mul(posW, gViewProjection);

	vout.NormalW = mul(vin.NormalL, (float3x3)gWorld);

	vout.TangentW = mul(vin.Tangent, (float3x3)gWorld);

	//	Flip UV.v
	vout.UV = vin.UV;

	return vout;
}

float4 PS_Main(VertexOut pin) : SV_Target
{
	float4 diffuse = gDiffuse;
	//if (gActivated_DiffuseTexture)
	//	diffuse = gDiffuseTexture2D.Sample(gsamAnisotropicWrap, pin.UV) * gDiffuse;

	//pin.NormalW = normalize(pin.NormalW);
	//float4 normalMapSample = gNormalTexture2D.Sample(gsamAnisotropicWrap, pin.UV);
	//float3 normalBumpedW = NormalSampleToWorldSpace(normalMapSample.rgb, pin.NormalW, pin.TangentW);

	//float3 toEyePosWorld = normalize(gEyePosWorld - pin.PosW);
	//float distanceToEye = length(toEyePosWorld);

	//float4 ambient = gAmbientLight * diffuse;

	//const float shininess = gShininess.x;
	//Material mat = { diffuse, gSpecular.xyz, shininess };
	//float3 shadowFactor = 1.0f;
	//float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyePosWorld, shadowFactor);

	//float4 lightColor = ambient + directLight;

	//lightColor.a = diffuse.a;

	//return lightColor;

	return diffuse;
}