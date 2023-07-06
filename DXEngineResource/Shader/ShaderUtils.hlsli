#include "LightingUtils.hlsli"

#define Max_Lights		   64
#define Max_UserDataInt		4
#define Max_UserDataFloat	4

Texture2D	 gDiffuseTexture2D		: register(t0);
Texture2D	 gSpecularTexture2D		: register(t1);
Texture2D	 gShininessTexture2D	: register(t2);
Texture2D	 gNormalTexture2D		: register(t3);

SamplerState gsamPointWrap			: register(s0);
SamplerState gsamPointClamp			: register(s1);
SamplerState gsamLinearWrap			: register(s2);
SamplerState gsamLinearClamp		: register(s3);
SamplerState gsamAnisotropicWrap	: register(s4);
SamplerState gsamAnisotropicClamp	: register(s5);
SamplerComparisonState gsamShadow	: register(s6);

cbuffer cbGameObject : register(b0)
{
	float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
	float4	 gDiffuse;
	float3	 gSpecular;
	float	 gShininess;
	float4	 gAmbient;

	int		 gDiffuseTextureOn;
	int		 gSpecularTextureOn;
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

	float4x4 gUserDataMatrix_0;
	float4x4 gUserDataMatrix_1;
	float4x4 gUserDataMatrix_2;
	float4x4 gUserDataMatrix_3;

	float4	 gUserDataFloat4s[16];
};

cbuffer cbLightGroup : register(b2)
{
	Light	 gLights[Max_Lights];
};

cbuffer cbCamera : register(b3)
{
	float4x4  gView;
	float4x4  gViewInv;
	float4x4  gProjection;
	float4x4  gProjectionInv;
	float4x4  gViewProjection;
	float4x4  gViewProjectionInv;

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

float GetShadowFactor(float4 positionW, float4x4 xformV, float4x4 xformP)
{
	uint width, height, numMips;
	gNormalTexture2D.GetDimensions(0, width, height, numMips);

	float dx = 1.0f / (float)width;
	const float2 offsets[9] =
	{
		float2(-dx, -dx),	float2(0.0f, -dx),	float2(+dx, -dx),
		float2(-dx, 0.0f),	float2(0.0f, 0.0f),	float2(+dx, 0.0f),
		float2(-dx, +dx),	float2(0.0f, +dx),	float2(+dx, +dx),
	};

	//	텍스처 변환 행렬을 곱한 후에는 반드시 원근 나누기를 수행해야 한다.
	float4 shadowH	= mul(mul(float4(positionW.xyz, 1.0f), xformV), xformP);
	float3 shadowUVD = shadowH.xyz / shadowH.w;

	shadowUVD.xy = (shadowUVD.xy + 1.0f) * 0.5f;
	shadowUVD.y = 1.0 - shadowUVD.y;

	float lightPercent = 0.0f;	
	//float shadowOffset = 0.000000275;
	float shadowOffset = 0.0000003;

	[unroll]
	for (int i = 0; i < 9; ++i)
	{
		lightPercent += gNormalTexture2D.SampleCmpLevelZero(gsamShadow, shadowUVD.xy + offsets[i], shadowUVD.z - shadowOffset).r;
	}

	return lightPercent / 9.0f;
}