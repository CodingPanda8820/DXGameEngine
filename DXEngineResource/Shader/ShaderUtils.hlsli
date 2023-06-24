#include "LightingUtils.hlsli"

#define Max_Lights		   64
#define Max_UserDataInt		4
#define Max_UserDataFloat	4

Texture2D	 gDiffuseTexture2D		: register(t0);
Texture2D	 gSpecularTexture2D		: register(t1);
Texture2D	 gAmbientTexture2D		: register(t2);
Texture2D	 gShininessTexture2D	: register(t3);
Texture2D	 gNormalTexture2D		: register(t4);

SamplerState gsamPointWrap			: register(s0);
SamplerState gsamPointClamp			: register(s1);
SamplerState gsamLinearWrap			: register(s2);
SamplerState gsamLinearClamp		: register(s3);
SamplerState gsamAnisotropicWrap	: register(s4);
SamplerState gsamAnisotropicClamp	: register(s5);

cbuffer cbGameObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float4	 gDiffuse;
	float4	 gSpecular;
	float4	 gAmbient;
	float4	 gShininess;

	int		 gDiffuseTextureOn;
	int		 gSpecularTextureOn;
	int		 gAmbientTextureOn;
	int		 gShininessTextureOn;
	int		 gNormalTextureOn;

	int		 gUserDataInt_0;
	int		 gUserDataInt_1;
	int		 gUserDataInt_2;
	int		 gUserDataInt_3;

	float	 gUserDataFloat_0;
	float	 gUserDataFloat_1;
	float	 gUserDataFloat_2;
	float	 gUserDataFloat_3;
};

cbuffer cbLightGroup : register(b2)
{
	Light	 gLights[Max_Lights];
};

cbuffer cbCamera : register(b3)
{
	float4x4  gView;
	float4x4  gProjection;
	float4x4  gViewProjection;

	float3	  gEyePosWorld;
	float	  gEyePosWorldPad1;

	float4	  gFogColor;
	float	  gFogStart;
	float	  gFogEnd;

	float2	  gResolution;
};

cbuffer cbScene : register(b4)
{
	float	gTotalTime;
	float	gDeltaTime;
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	//	Uncompress each component from [0, 1] to [-1, 1].
	float3 normalT = 2.0f * normalMapSample - 1.0f;

	//	Build orthonormal basis.
	float3 N = unitNormalW;
	float3 T = normalize(tangentW - dot(tangentW, N) * N);
	float3 B = cross(N, T);

	float3x3 TBN = float3x3(T, B, N);

	//	Transform from tangent space to world space.
	float3 bumpedNormalW = mul(normalT, TBN);

	return bumpedNormalW;
}