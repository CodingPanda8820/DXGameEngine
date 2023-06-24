#pragma once
#include "EngineUtils.h"

class Transform
{
public:
	Transform();
	~Transform();

public:
	void Init();
	void Update();

public:
	void UpdateTranslate(const XMFLOAT3& translate);
	void UpdateTranslate(const XMVECTOR& translate);
	void UpdateTranslate(float tx, float ty, float tz);
	void UpdateRotate(const XMFLOAT3& rotate);
	void UpdateRotate(const XMVECTOR& rotate);
	void UpdateRotate(float rx, float ry, float rz);
	void UpdateScale(const XMFLOAT3& scale);
	void UpdateScale(const XMVECTOR& scale);
	void UpdateScale(float sx, float sy, float sz);

public:
	void SetName(const string& name);
	void SetTranslate(const XMFLOAT3& translate);
	void SetTranslate(const XMVECTOR& translate);
	void SetTranslate(float tx, float ty, float tz);
	void SetRotate(const XMFLOAT3& rotate);
	void SetRotate(const XMVECTOR& rotate);
	void SetRotate(float rx, float ry, float rz);
	void SetScale(const XMFLOAT3& scale);
	void SetScale(const XMVECTOR& scale);
	void SetScale(float sx, float sy, float sz);

	string		GetName();
	XMFLOAT3	GetTranslate();
	XMVECTOR	GetTranslateAsVector();
	XMFLOAT3	GetRotate();
	XMVECTOR	GetRotateAsVector();
	XMFLOAT3	GetScale();
	XMVECTOR	GetScaleAsVector();

	XMFLOAT4X4	GetLocal();
	XMMATRIX	GetWorld();

	XMVECTOR	GetRight();
	XMVECTOR	GetUp();
	XMVECTOR	GetLook();

	shared_ptr<Transform> GetParent();
	void SetParent(shared_ptr<Transform> parent);
	void IndependantFromParent();

private:
	void UpdateLocal();

private:
	string		m_name;
		
	XMFLOAT3	m_translate;
	XMFLOAT3	m_rotate;
	XMFLOAT3	m_scale;

	XMFLOAT4X4	m_local;
	XMFLOAT4X4	m_localScale;
	XMFLOAT4X4	m_localRotation;
	XMFLOAT4X4	m_localTranslation;

	shared_ptr<Transform>	m_parent;
};

