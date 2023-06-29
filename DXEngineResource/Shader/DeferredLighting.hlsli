#include "ShaderUtils.hlsli"

// Author	: Shin yongseob(CodingPanda8820@gmail.com)
// Data		: 2023.06.11 Sunday
// Information
//	gUserDataInt[0] : Index of Light
//  gUserDataInt[1] : LightType
//

struct VertexIn
{
	float3 position  : POSITION;
	float2 uv		 : TEXCOORD;
};

struct VertexOut
{
	float4 position : SV_Position;
	float2 uv		: TEXCOORD;
};

struct PixelOut
{
	float4 diffuse  : SV_Target0;
	float4 specular : SV_Target1;
};

//	static : 해당 변수가 C++ 응용 프로그램에 노출되지 않는다.
static const float2 BasePosition[4] =
{
	float2(-1.0, 1.0),
	float2(1.0, 1.0),
	float2(-1.0, -1.0),
	float2(1.0, -1.0)
};

VertexOut VS_DirectionalLight(uint VertexID : SV_VertexID)
{
	VertexOut vout = (VertexOut)0;
	vout.position = float4(BasePosition[VertexID].xy, 0.0f, 1.0f);

	//	Flip UV.y
	vout.uv = mul(BasePosition[VertexID].xy, 0.5) + 0.5;
	vout.uv.y = abs(vout.uv.y - 1.0f);

	return vout;
}

PixelOut PS_DirectionalLight(VertexOut pin)
{
	Material material	= (Material)0;
	material.Position	= gShininessTexture2D.Sample(gsamLinearClamp, pin.uv);
	material.Diffuse	= gDiffuseTexture2D.Sample(gsamLinearClamp, pin.uv).xyz;
	material.Specular	= gDiffuseTexture2D.Sample(gsamLinearClamp, pin.uv).w;
	material.Shininess	= gSpecularTexture2D.Sample(gsamLinearClamp, pin.uv).w;
	material.Normal		= gSpecularTexture2D.Sample(gsamLinearClamp, pin.uv).xyz;

	if (length(material.Diffuse) == 0.0f)
		clip(-1);

	//	Light : Diffuse & Specular
	int		lightIndex = gUserDataInt_1;
	float3	toEye = normalize(gEyePosWorld - material.Position);
	LightColor color = ComputeDirectionalLight(gLights[lightIndex], material, toEye);

	//	Light : Shadow
	float shadowFactor = GetShadowFactor(material.Position, gUserDataMatrix_0, gUserDataMatrix_1);

	PixelOut pout = (PixelOut)0;
	pout.diffuse = float4(color.diffuse, 1.0f) * shadowFactor;
	pout.specular = float4(color.specular, 1.0f) * shadowFactor;

	return pout;
}

VertexOut VS_PointLight(uint VertexID : SV_VertexID)
{
	VertexOut vout = (VertexOut)0;
	vout.position = float4(BasePosition[VertexID].xy, 0.0f, 1.0f);

	//	Flip UV.y
	vout.uv = mul(BasePosition[VertexID].xy, 0.5) + 0.5;
	vout.uv.y = abs(vout.uv.y - 1.0f);

	return vout;
}

PixelOut PS_PointLight(VertexOut pin)
{
	Material material = (Material)0;
	material.Position = gShininessTexture2D.Sample(gsamLinearClamp, pin.uv);
	material.Diffuse = gDiffuseTexture2D.Sample(gsamLinearClamp, pin.uv).xyz;
	material.Specular = gDiffuseTexture2D.Sample(gsamLinearClamp, pin.uv).w;
	material.Shininess = gSpecularTexture2D.Sample(gsamLinearClamp, pin.uv).w;
	material.Normal = gSpecularTexture2D.Sample(gsamLinearClamp, pin.uv).xyz;

	int		lightIndex = gUserDataInt_0;
	float3	toEye = normalize(gEyePosWorld - material.Position);

	LightColor color = ComputePointLight(gLights[lightIndex], material, toEye);

	PixelOut pout = (PixelOut)0;
	pout.diffuse = float4(color.diffuse, 1.0f);
	pout.specular = float4(color.specular, 1.0f);

	return pout;
}

//	Ambient Light
VertexOut VS_AmbientLight(uint VertexID : SV_VertexID)
{
	VertexOut vout = (VertexOut)0;
	vout.position = float4(BasePosition[VertexID].xy, 0.0f, 1.0f);

	//	Flip UV.y
	vout.uv = mul(BasePosition[VertexID].xy, 0.5) + 0.5;
	vout.uv.y = abs(vout.uv.y - 1.0f);

	return vout;
}

float4 PS_AmbientLight(VertexOut pin) : SV_Target
{
	float4 strength = float4(gLights[gUserDataInt_0].Strength, 0.0f);
	float4 diffuseAlbedo = float4(gDiffuseTexture2D.Sample(gsamPointWrap, pin.uv).xyz, 1.0f);

	float4 ambientLight = diffuseAlbedo * strength;
	float4 diffuseLight = gSpecularTexture2D.Sample(gsamPointWrap, pin.uv);
	float4 SpecularLight = gShininessTexture2D.Sample(gsamPointWrap, pin.uv);

	float4 finalColor = ambientLight + (SpecularLight * diffuseLight);

	if (finalColor.x == 0.0f && finalColor.y == 0.0f && finalColor.z == 0.0f)
		clip(-1);

	return finalColor;
}
