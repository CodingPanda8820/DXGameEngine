#include "pch.h"
#include "LightShape.h"

LightShape::LightShape()
{
	m_strength = { 0.5f, 0.5f, 0.5f };
	m_falloffStart = 1.0f;
	m_falloffEnd = 10.0f;
	m_spotPower = 64.0f;
}

LightShape::~LightShape()
{
}

void LightShape::Create(const string& name)
{
	m_name = name;
}

string LightShape::GetName()
{
	return m_name;
}
XMFLOAT3 LightShape::GetStrength()
{
	return m_strength;
}

float LightShape::GetFalloffStart()
{
	return m_falloffStart;
}

float LightShape::GetFalloffEnd()
{
	return m_falloffEnd;
}

float LightShape::GetSpotPower()
{
	return m_spotPower;
}

void LightShape::SetName(const string& name)
{
	m_name = name;
}

void LightShape::SetStrength(const XMFLOAT3& strength)
{
	m_strength = strength;
}

void LightShape::SetStrength(float x, float y, float z)
{
	m_strength.x = x;
	m_strength.y = y;
	m_strength.z = z;
}

void LightShape::SetFalloffStart(float value)
{
	m_falloffStart = value;
}

void LightShape::SetFalloffEnd(float value)
{
	m_falloffEnd = value;
}

void LightShape::SetSpotPower(float value)
{
	m_spotPower = value;
}
