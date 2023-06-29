#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3	PositionL	: POSITION;
	float3	NormalL		: NORMAL;
	float2	UV			: TEXCOORD;
};

struct VertexOut
{
	float4 PositionH	: SV_Position;
	float4 PositionW	: POSITION;
	float3 NormalW		: NORMAL;
	float2 UV			: TEXCOORD;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0;

	float4 PositionW = mul(float4(vin.PositionL, 1.0f), gWorld);
	float3 NormalW = normalize(mul(float4(vin.NormalL, 0.0f), gWorld)).xyz;

	vout.PositionH = mul(PositionW, gViewProjection);
	vout.PositionW = PositionW;
	vout.NormalW = normalize(NormalW.xyz);
	vout.UV = vin.UV;

	return vout;
}

struct PixelOut
{
	float4 Position			  : SV_Target0;
	float4 DiffuseAndSpecular : SV_Target1;
	float4 NormalAndShininess : SV_Target2;
};

PixelOut PS_Main(VertexOut pin)
{
	PixelOut pout = (PixelOut)0;

	float4 diffuse = gDiffuse;
	if (gDiffuseTextureOn)
		diffuse = gDiffuseTexture2D.Sample(gsamAnisotropicWrap, pin.UV) * gDiffuse;

	float3 specular = gSpecular;
	if (gSpecularTextureOn)
		specular = gSpecularTexture2D.Sample(gsamAnisotropicWrap, pin.UV).xyz * gSpecular;

	float3 normal = pin.NormalW;
	if (gNormalTextureOn)
	{
		float3 normalTexture2D = gNormalTexture2D.Sample(gsamLinearWrap, pin.UV).xyz;
		normal = (normalTexture2D + 1.0f) * 0.5f;
	}

	float shininess = gShininess;
	if (gShininessTextureOn)
		shininess = gShininessTexture2D.Sample(gsamAnisotropicWrap, pin.UV).x * gShininess;

	pout.Position = float4(pin.PositionW.xyz, pin.PositionH.z / pin.PositionH.w);
	pout.DiffuseAndSpecular = float4(diffuse.xyz, specular.x);
	pout.NormalAndShininess = float4(normal.xyz, shininess.x);

	return pout;
}