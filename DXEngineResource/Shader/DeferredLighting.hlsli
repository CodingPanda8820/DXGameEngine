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
	VertexOut vout  = (VertexOut)0;
	vout.position   = float4(BasePosition[VertexID].xy, 0.0f, 1.0f);

	//	Flip UV.y
	vout.uv	  = mul(BasePosition[VertexID].xy, 0.5) + 0.5;
	vout.uv.y = abs(vout.uv.y - 1.0f);

	return vout;
}

PixelOut PS_DirectionalLight(VertexOut pin)
{
	float3 positionV = gDiffuseTexture2D.Sample(gsamLinearClamp, pin.uv).xyz;
	if (positionV.z <= 0.f)
		clip(-1);

	float3 normalV = gNormalTexture2D.Sample(gsamLinearClamp, pin.uv).xyz;

	Material mat		= (Material)0;
	mat.DiffuseColor	= positionV;
	mat.Normal			= normalV;

	int		lightIndex	= gUserDataInt_0;
	float3	toEye		= normalize(gEyePosWorld - gLights[lightIndex].Position);

	LightColor color = ComputeDirectionalLight(gLights[lightIndex], mat, toEye);

	PixelOut pout = (PixelOut)0;
	pout.diffuse  = float4(color.diffuse,  1.0f);
	pout.specular = float4(color.specular, 1.0f);

	return pout;
}

//	Ambient Light
//  Input
//	Texture[0] : GBuffer::Diffuse
//	Texture[1] : Light::Diffuse
//	Texture[2] : Light::Specular
//	Output	   : float4 DeferredOutput
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
	float4 ambient = float4(gLights[gUserDataInt_0].Strength, 0.0f);

	float4 color		 = gDiffuseTexture2D.Sample(gsamPointWrap, pin.uv);
	float4 lightColor	 = gSpecularTexture2D.Sample(gsamPointWrap, pin.uv);
	float4 lightSpecular = gAmbientTexture2D.Sample(gsamPointWrap, pin.uv);

	if (lightColor.x == 0.0f && lightColor.y == 0.0f && lightColor.z == 0.0f)
		clip(-1);	

	//return (color * lightColor) + lightSpecular + ambient;
	return (color * ambient) + lightColor;
}
