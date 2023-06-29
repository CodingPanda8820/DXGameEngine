#pragma once
#include "EngineUtils.h"

class Transform : public enable_shared_from_this<Transform>
{
public:
	Transform();
	~Transform();

public:
	void Init();
	void Update();

public:
	void UpdateScale(const XMFLOAT3& scale);
	void UpdateScale(const XMVECTOR& scale);
	void UpdateScale(float sx, float sy, float sz);
	void UpdateRotate(const XMFLOAT3& rotate);
	void UpdateRotate(const XMVECTOR& rotate);
	void UpdateRotate(float rx, float ry, float rz);
	void UpdateTranslate(const XMFLOAT3& translate);
	void UpdateTranslate(const XMVECTOR& translate);
	void UpdateTranslate(float tx, float ty, float tz);

public:
	void SetName(const string& name);

	void SetScale(const XMFLOAT3& scale);
	void SetScale(const XMVECTOR& scale);
	void SetScale(float sx, float sy, float sz);
	void SetRotate(const XMFLOAT3& rotate);
	void SetRotate(const XMVECTOR& rotate);
	void SetRotate(float rx, float ry, float rz);
	void SetTranslate(const XMFLOAT3& translate);
	void SetTranslate(const XMVECTOR& translate);
	void SetTranslate(float tx, float ty, float tz);

	string		GetName();

	//	Local Transform Information
	XMFLOAT3	GetLocalScaleFloat3();
	XMVECTOR	GetLocalScaleVector();
	XMFLOAT3	GetLocalRotateFloat3();
	XMVECTOR	GetLocalRotateVector();
	XMFLOAT3	GetLocalTranslateFloat3();
	XMVECTOR	GetLocalTranslateVector();

	XMFLOAT3	GetLocalRollFloat3();
	XMVECTOR	GetLocalRollVector();
	XMFLOAT3	GetLocalPitchFloat3();
	XMVECTOR	GetLocalPitchVector();
	XMFLOAT3	GetLocalYawFloat3();
	XMVECTOR	GetLocalYawVector();
	XMFLOAT3	GetLocalPivotFloat3();
	XMVECTOR	GetLocalPivotVector();

	XMMATRIX	GetLocalTransformMatrix();
	XMFLOAT4X4	GetLocalTransformFloat4x4();

	//	World Transform Information
	XMFLOAT3	GetWorldRollFloat3();
	XMVECTOR	GetWorldRollVector();
	XMFLOAT3	GetWorldPitchFloat3();
	XMVECTOR	GetWorldPitchVector();
	XMFLOAT3	GetWorldYawFloat3();
	XMVECTOR	GetWorldYawVector();
	XMFLOAT3	GetWorldPivotFloat3();
	XMVECTOR	GetWorldPivotVector();

	XMFLOAT4X4	GetWorldTransformFloat4x4();
	XMMATRIX	GetWorldTransformMatrix();
	XMFLOAT4X4	GetWorldViewTransformFloat4x4();
	XMMATRIX	GetWorldViewTransformMatrix();
	XMFLOAT4X4	GetWorldProjectionTransformFloat4x4(float fovY, float resW, float resH, float nearZ, float farZ, PROJECTION_TYPE projectionType = PROJECTION_TYPE::PERSPECTIVE);
	XMMATRIX	GetWorldProjectionTransformMatrix(float fovY, float resW, float resH, float nearZ, float farZ, PROJECTION_TYPE projectionType = PROJECTION_TYPE::PERSPECTIVE);

	shared_ptr<Transform> GetParent();
	void SetParent(shared_ptr<Transform> parent);
	void IndependantFromParent();

private:
	void UpdateLocalTransform();

private:
	string		m_name;

	XMFLOAT3	m_translate;
	XMFLOAT3	m_rotate;
	XMFLOAT3	m_scale;

	XMFLOAT4X4	m_local;
	XMFLOAT4X4	m_localScale;
	XMFLOAT4X4	m_localRotation;
	XMFLOAT4X4	m_localTranslation;

	XMFLOAT4X4	m_localTransform;

	shared_ptr<Transform>	m_parent;
};

