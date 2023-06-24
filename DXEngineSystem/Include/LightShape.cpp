#include "pch.h"
#include "LightShape.h"

LightShape::LightShape()
{
	m_position	= { 0.0f, 0.0f, 0.0f };
	m_strength	= { 0.5f, 0.5f, 0.5f };
	m_direction	= { 0.0f, -1.0f, 0.0f };

	m_falloffStart	= 1.0f;
	m_falloffEnd	= 10.0f;
	m_spotPower		= 64.0f;
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

XMFLOAT3 LightShape::GetPosition()
{
	return m_position;
}

XMVECTOR LightShape::GetPositionAsVector()
{
	return XMLoadFloat3(&m_position);
}

XMFLOAT3 LightShape::GetStrength()
{
	return m_strength;
}

XMFLOAT3 LightShape::GetDirection()
{
	return m_direction;
}

XMVECTOR LightShape::GetDirectionAsVector()
{
	return XMLoadFloat3(&m_direction);
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

void LightShape::SetPosition(const XMFLOAT3& position)
{
	m_position = position;
}

void LightShape::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
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

void LightShape::SetDirection(const XMFLOAT3& direction)
{
	m_direction = direction;
}

void LightShape::SetDirection(float x, float y, float z)
{
	m_direction.x = x;
	m_direction.y = y;
	m_direction.z = z;
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
