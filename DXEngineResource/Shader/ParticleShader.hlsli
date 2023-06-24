#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "ShaderUtils.hlsli"

struct VertexIn
{
	float3	PosL		: POSITION;
	float	Radius		: RADIUS;
	float3	NormalL		: NORMAL;
	float	Life		: LIFE;
	float3	TangentU	: TANGENT;
	float	age			: AGE;
	float2	UV			: TEXCOORD;
};

struct VertexOut
{
	float3	CenterW	: POSITION;
	float	Radius  : RADIUS;
};

struct GeometryOut
{
	float4	PosH	: SV_POSITION;
	float3	PosW	: POSITION;
	float3	NormalW : NORMAL;
	float2	UV		: TEXCOORD;
	uint	PrimID	: SV_PrimitiveID;
};

VertexOut VS_Main(VertexIn vin)
{
	VertexOut vout;
	vout.CenterW = mul(float4(vin.PosL, 1.0f), gWorld).xyz;
	vout.Radius = vin.Radius;

	return vout;
}

[maxvertexcount(4)]
void GS_Main(point VertexOut gin[1], uint primID : SV_PrimitiveID, inout TriangleStream<GeometryOut> ioStream)
{
	float3 up	= float3(0.0f, 1.0f, 0.0f);
	float3 look = gEyePosWorld - gin[0].CenterW;
	
	look.y = 0.0f;
	look = normalize(look);
	
	float3 right = cross(up, look);
	float radius = gin[0].Radius;

	float4 v[4];
	v[0] = float4(gin[0].CenterW + radius * right - radius * up, 1.0f);
	v[1] = float4(gin[0].CenterW + radius * right + radius * up, 1.0f);
	v[2] = float4(gin[0].CenterW - radius * right - radius * up, 1.0f);
	v[3] = float4(gin[0].CenterW - radius * right + radius * up, 1.0f);

	float2 UV[4] =
	{
		float2(0.0f, 1.0f),
		float2(0.0f, 0.0f),
		float2(1.0f, 1.0f),
		float2(1.0f, 0.0f)
	};

	GeometryOut gout;
	[unroll]
	for (int i = 0; i < 4; ++i)
	{
		gout.PosH	 = mul(v[i], gViewProjection);
		gout.PosW	 = v[i].xyz;
		gout.NormalW = look;
		gout.UV		 = UV[i];
		gout.PrimID = primID;

		ioStream.Append(gout);
	}
}

float4 PS_Main(GeometryOut pin) : SV_Target
{
	float3 uvw = float3(pin.UV, pin.PrimID % 3);

	float4 diffuse = gDiffuse;
	if(gActivated_DiffuseTexture)
		diffuse = gDiffuseTexture2D.Sample(gsamAnisotropicWrap, pin.UV) * gDiffuse;

	pin.NormalW = normalize(pin.NormalW);

	float3	toEyeW = gEyePosWorld - pin.PosW;
	float	distanceToEye = length(toEyeW);

	toEyeW /= distanceToEye;
	
	float4	ambient = gAmbientLight * diffuse;
	float3	specular = mul(gSpecular.xyz, gSpecular.a);
	float	shininess = gShininess.a;
	Material mat = { diffuse, specular, shininess };

	float3 shadowFactor = 1.0f;
	float4 directLight = ComputeLighting(gLights, mat, pin.PosW, pin.NormalW, toEyeW, shadowFactor);

	float4 lgtColor = ambient + directLight;

	lgtColor.a = diffuse.a;

	return lgtColor;
}