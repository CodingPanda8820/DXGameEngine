#define LIGHT_TYPE_AMBIENT		0
#define LIGHT_TYPE_DIRECTIONAL	1
#define LIGHT_TYPE_POINT		2
#define LIGHT_TYPE_SPOT			3

struct Light
{
	float3	 Strength;
	float	 FalloffStart;
	float3	 Direction;
	float	 FalloffEnd;
	float3	 Position;
	float	 SpotPower;
};

struct LightColor
{
	float3 diffuse;
	float3 specular;
};

struct Material
{
	float3	Diffuse;
	float	Specular;
	float	Shininess;
	float3	Normal;
	float4	Position;
};

static const float4x4 NTexCoord =
float4x4(+0.5f, +0.0f, +0.0f, +0.0f,
	+0.0f, -0.5f, +0.0f, +0.0f,
	+0.0f, +0.0f, +1.0f, +0.0f,
	+0.5f, +0.5f, +0.0f, +1.0f);

float GetAttenuation(float d, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float ConvertDepthFromNDCToView(float NDCz, float4x4 xformProjection)
{
	float viewZ = xformProjection[3][2] / (NDCz - xformProjection[2][2]);

	return viewZ;
}

//	normal : normal in view space
//	depth  : depth in view space
float GetOcclusion(float fadeStart, float fadeEnd, float distZ, float surfaceEpsilon)
{
	float occlusion = 0.0f;
	if (distZ > surfaceEpsilon)
	{
		float fadeLength = fadeEnd - fadeStart;

		occlusion = saturate((fadeEnd - distZ) / fadeLength);
	}

	return occlusion;
}

// float3 p = (depth / position.z) * position;
// float3 randomVec = 2.0f * gRandomVectorMap.SampleLevel(gsamLinearWrap, 4.0f * uv, 0.0f).rgb - 1.0f;
float SSAO(float4 p, float rz, float4x4 uv, float3 n, float3 randomVec,
		float4 offsetVectors[14], float radius, float fadeStart, float fadeEnd, float surfaceEpsilon)
{
	float sumOcclusion = 0.0f;
	for (int i = 0; i < 14; ++i)
	{
		float3	offset = reflect(offsetVectors[i].xyz, randomVec);
		float	flip = sign(dot(offset, n));
		float3	q = p + flip * radius * offset;
		float4	qProjection = mul(float4(q, 1.0f), uv);
		qProjection /= qProjection.w;

		float3 r  = (rz / q.z) * q;

		float distZ = p.z - r.z;
		float dp = max(dot(n, normalize(r - p)), 0.0f);
		float occlusion = dp * GetOcclusion(fadeStart, fadeEnd, distZ, surfaceEpsilon);

		sumOcclusion += occlusion;
	}

	sumOcclusion /= 14;

	float access = 1.0f - sumOcclusion;

	return saturate(pow(access, 6.0f));
}

float SchlickFresnel(float R0, float3 normal, float3 toLight)
{
	float cosIncidentAngle = saturate(dot(normal, toLight));

	float f0 = 1.0f - cosIncidentAngle;
	float reflectPercent = R0 + (1.0f - R0) * (f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

float3 BlinnPhong(float3 lightStrength, float3 toLight, float3 toEye, Material material)
{
	// It will be simillar with diffuse reflect when shininess is closed to value of 0.0
	const float m = material.Shininess * 256.0f;
	float3 halfVec = normalize(toEye + toLight);

	float	roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, material.Normal), 0.0f), m) / 8.0f;
	float	fresnelFactor = SchlickFresnel(material.Specular, halfVec, toLight);

	float3	specular = fresnelFactor * roughnessFactor;

	specular = lightStrength * (specular / (specular + 1.0f));

	return specular * lightStrength;
}



LightColor ComputeDirectionalLight(Light lgt, Material mat, float3 toEye)
{
	float3 toLight = -lgt.Direction;

	float  ndotl = normalize(max(dot(toLight, mat.Normal), 0.0f));
	float3 lambertCosine = saturate(lgt.Strength * ndotl);

	LightColor color = (LightColor)0;
	color.diffuse = lambertCosine * mat.Diffuse;
	color.specular = BlinnPhong(lambertCosine, toLight, toEye, mat);

	return color;
}

LightColor ComputePointLight(Light lgt, Material mat, float3 toEye)
{
	LightColor color = (LightColor)0;
	color.diffuse = float3(1.0f, 1.0f, 0.0f);
	color.specular = float3(0.0f, 1.0f, 0.0f);

	return color;
}

LightColor ComputeSpotLight(Light lgt, Material mat, float3 toEye)
{
	LightColor color = (LightColor)0;
	color.diffuse = float3(1.0f, 0.0f, 1.0f);
	color.specular = float3(0.0f, 0.0f, 1.0f);

	return color;
}

//float3 ComputePointLight(Light lgt, Material mat, float3 pos, float3 normal, float3 toEye)
//{
//	float3 lightVec = lgt.Position - pos;
//
//	float d = length(lightVec);
//
//	if (d > lgt.FalloffEnd)
//		return 0.0f;
//
//	lightVec /= d;
//
//	float ndotl = max(dot(lightVec, normal), 0.0f);
//	float3 lightStrength = lgt.Strength * ndotl;
//
//	float att = GetAttenuation(d, lgt.FalloffStart, lgt.FalloffEnd);
//	lightStrength *= att;
//
//	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
//}

//float3 ComputeSpotLight(Light lgt, Material mat, float3 pos, float3 normal, float3 toEye)
//{
//	float3 lightVec = lgt.Position - pos;
//
//	float d = length(lightVec);
//
//	if (d > lgt.FalloffEnd)
//		return 0.0f;
//
//	lightVec /= d;
//
//	float ndotl = max(dot(lightVec, normal), 0.0f);
//	float3 lightStrength = lgt.Strength * ndotl;
//
//	float att = GetAttenuation(d, lgt.FalloffStart, lgt.FalloffEnd);
//	lightStrength *= att;
//
//	float spotFactor = pow(max(dot(-lightVec, lgt.Direction), 0.0f), lgt.SpotPower);
//	lightStrength *= spotFactor;
//
//	return BlinnPhong(lightStrength, lightVec, normal, toEye, mat);
//}