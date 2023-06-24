#include "pch.h"
#include "EngineMath.h"

const float EngineMath::Infinity = FLT_MAX;
const float EngineMath::Pi = 3.1415926535f;

float EngineMath::AngleFromXY(float x, float y)
{
	float theta = 0.0f;

	if (x >= 0.0f)
	{
		theta = atanf(y / x);

		if (theta < 0.0f)
			theta += 2.0f * Pi;
	}
	else
		theta = atanf(y / x) + Pi;

	return theta;
}

XMVECTOR EngineMath::RandUnitVec3()
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		return XMVector3Normalize(v);
	}
}

XMVECTOR EngineMath::RandHemisphereUnitVec3(XMVECTOR n)
{
	XMVECTOR One = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	XMVECTOR Zero = XMVectorZero();

	while (true)
	{
		XMVECTOR v = XMVectorSet(RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), RandF(-1.0f, 1.0f), 0.0f);

		if (XMVector3Greater(XMVector3LengthSq(v), One))
			continue;

		if (XMVector3Less(XMVector3Dot(n, v), Zero))
			continue;

		return XMVector3Normalize(v);
	}
}

float EngineMath::DegreeToRadian(float value)
{
	return value * (Pi / 180);
}

XMFLOAT3 EngineMath::DegreesToRadians(float x, float y, float z)
{
	float X = DegreeToRadian(x);
	float Y = DegreeToRadian(y);
	float Z = DegreeToRadian(z);

	XMFLOAT3 Radians = { X, Y, Z };

	return Radians;
}

XMFLOAT3 EngineMath::DegreesToRadians(const XMFLOAT3& degrees)
{
	return DegreesToRadians(degrees.x, degrees.y, degrees.z);
}

XMFLOAT3 EngineMath::DegreesToRadians(const XMVECTOR& degrees)
{
	return DegreesToRadians(XMVectorGetX(degrees),
							XMVectorGetY(degrees),
							XMVectorGetZ(degrees));
}

float EngineMath::RadianToDegree(float value)
{
	return value * (180 / Pi);
}

XMFLOAT3 EngineMath::RadiansToDegrees(float x, float y, float z)
{
	float X = RadianToDegree(x);
	float Y = RadianToDegree(y);
	float Z = RadianToDegree(z);

	XMFLOAT3 Degrees = { X, Y, Z };

	return Degrees;
}

XMFLOAT3 EngineMath::RadiansToDegrees(const XMFLOAT3& radians)
{
	return RadiansToDegrees(radians.x, radians.y, radians.z);
}

XMFLOAT3 EngineMath::RadiansToDegrees(const XMVECTOR& radians)
{
	return RadiansToDegrees(XMVectorGetX(radians), XMVectorGetY(radians), XMVectorGetZ(radians));
}
