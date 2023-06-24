// �ں��Ʈ ����Ʈ(Ambient Light) :	�� ��ü�� ������ ��ġ�� �ݱ��� ����(Hemispheric Lighting)����
//									��� ���� ���� ���� ���� ���̵� �ں��Ʈ ����Ʈ�� �����Ѵ�.
//									ǥ�鿡�� �� �� �̻� ����Ǵ� ���� ���� ������ ������ �����.
// 
// �𷺼ų� ����Ʈ(Directional Light) :	�ſ� �� �Ÿ��� �ִ� ������ ��� ������ �����ϰ� �����Ѵٰ� �����Ѵ�.
//										����, ����, ������ ����Ѵ�.
// 
// ����Ʈ ����Ʈ(Point Light) :	��� �������� ���������� ����
// 
// ����Ʈ ����Ʈ(Spot Light)  :	Ư�� �������� ���� ������� ����Ǵ� �ձ� ����
//

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
	float3	DiffuseColor;
	float	SpecularIntensity;
	float	Shininess;
	float3	Normal;
};

float GetAttenuation(float d, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float3 SchlickFresnel(float3 R0, float3 normal, float3 lightVec)
{
	float cosIncidentAngle = saturate(dot(normal, lightVec));

	float f0 = 1.0f - cosIncidentAngle;
	float3 reflectPercent = R0 + (1.0f - R0) *(f0 * f0 * f0 * f0 * f0);

	return reflectPercent;
}

float3 BlinnPhong(float3 strength, float3 direction, float3 toEye, Material material)
{
	const float m	= material.Shininess * 256.0f;
	float3 halfVec	= normalize(toEye + direction);

	float	roughnessFactor = (m + 8.0f) * pow(max(dot(halfVec, material.Normal), 0.0f), m) / 8.0f;
	float3	fresnelFactor	= SchlickFresnel(material.SpecularIntensity, halfVec, direction);

	float3	specular = fresnelFactor * roughnessFactor;

	specular = specular / (specular + 1.0f);

	return specular * strength;
}

LightColor ComputeAmbientLight(Light lgt, Material mat, float3 toEye)
{
	LightColor color = (LightColor)0;
	color.diffuse	 = float3(1.0f, 0.0f, 0.0f);
	color.specular	 = float3(0.0f, 1.0f, 0.0f);

	return color;
}

LightColor ComputeDirectionalLight(Light lgt, Material mat, float3 toEye)
{
	float3 lightVec = -lgt.Direction;

	float  ndotl = normalize(max(dot(lightVec, mat.Normal), 0.0f));
	float3 lambertCosine = saturate(lgt.Strength * ndotl);

	LightColor color = (LightColor)0;
	color.diffuse	 = lambertCosine;
	color.specular	 = BlinnPhong(lambertCosine, lightVec, toEye, mat);

	return color;
}

LightColor ComputePointLight(Light lgt, Material mat, float3 toEye)
{
	LightColor color = (LightColor)0;
	color.diffuse	 = float3(1.0f, 1.0f, 0.0f);
	color.specular	 = float3(0.0f, 1.0f, 0.0f);

	return color;
}

LightColor ComputeSpotLight(Light lgt, Material mat, float3 toEye)
{
	LightColor color = (LightColor)0;
	color.diffuse	 = float3(1.0f, 0.0f, 1.0f);
	color.specular	 = float3(0.0f, 0.0f, 1.0f);

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