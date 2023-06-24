#pragma once
#include "EngineUtils.h"
#include "Transform.h"
#include "GPUBufferUtils.h"

class LightShape
{
public:
	LightShape();
	~LightShape();

public:
	void Create(const string& name);

	string				GetName();
	XMFLOAT3			GetPosition();
	XMVECTOR			GetPositionAsVector();
	XMFLOAT3			GetDirection();
	XMVECTOR			GetDirectionAsVector();
	XMFLOAT3			GetStrength();
	float				GetFalloffStart();
	float				GetFalloffEnd();
	float				GetSpotPower();

	void SetName(const string& name);
	void SetPosition(const XMFLOAT3& position);
	void SetPosition(float x, float y, float z);
	void SetStrength(const XMFLOAT3& strength);
	void SetStrength(float x, float y, float z);
	void SetDirection(const XMFLOAT3& direction);
	void SetDirection(float x, float y, float z);
	void SetFalloffStart(float value);
	void SetFalloffEnd(float value);
	void SetSpotPower(float value);

private:
	string				m_name;

	XMFLOAT3			m_position;
	XMFLOAT3			m_strength;
	XMFLOAT3			m_direction;

	float				m_falloffStart;
	float				m_falloffEnd;

	float				m_spotPower;
};

