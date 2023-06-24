#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3	PositionL	: POSITION;
	float3	NormalL		: NORMAL;
	float3	TangentL	: TANGENT;
	float2	UV			: TEXCOORD;
};

struct VertexOut
{
	float4 PositionWVP	: SV_Position;
	float3 PositionWV	: POSITION;
	float3 NormalWV		: NORMAL;
	float3 TangentWV	: TANGENT;
	float3 BinormalWV	: BINORMAL;
	float2 UV			: TEXCOORD;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout = (VertexOut)0;

	float4 PositionW	= mul(float4(vin.PositionL, 1.0f), gWorld);
	float3 NormalW		= normalize(mul(float4(vin.NormalL, 0.0f), gWorld)).xyz;
	float3 TangentW		= normalize(mul(float4(vin.TangentL, 0.0f), gWorld)).xyz;

	vout.PositionWVP = mul(PositionW, gViewProjection);
	vout.PositionWV	 = mul(PositionW, gView).xyz;
	vout.NormalWV	 = normalize(mul(float4(NormalW, 0.0f), gView)).xyz; 
	vout.TangentWV	 = normalize(mul(float4(TangentW, 0.0f), gView)).xyz;
	vout.BinormalWV	 = normalize(cross(vout.TangentWV, vout.NormalWV)).xyz;
	vout.UV = vin.UV;

	return vout;
}

struct PixelOut
{
	float4 Position : SV_Target0;
	float4 Normal	: SV_Target1;
	float4 Diffuse	: SV_Target2;
};

PixelOut PS_Main(VertexOut pin)
{
	PixelOut pout = (PixelOut)0;

	float4 diffuse = gDiffuse;
	if(gDiffuseTextureOn)
		diffuse = gDiffuseTexture2D.Sample(gsamAnisotropicWrap, pin.UV) * gDiffuse;

	float3 normalWV = pin.NormalWV;
	if (gNormalTextureOn)
	{
		//	[0, 255] 범위에서 [0, 1]로 변환
		float3 tangentSpaceNormal = gNormalTexture2D.Sample(gsamAnisotropicWrap, pin.UV).xyz;
		//	[0, 1] 범위에서 [-1, 1]로 변환
		tangentSpaceNormal = (tangentSpaceNormal - 0.5f) * 2.0f;
		float3x3 mat = { pin.TangentWV, pin.BinormalWV, pin.NormalWV };
		normalWV = normalize(mul(tangentSpaceNormal, mat));
	}

	pout.Position = float4(pin.PositionWV.xyz, 0.0f);
	pout.Normal	  = float4(pin.NormalWV.xyz, 0.0f);
	pout.Diffuse  = diffuse;

	return pout;
}