#include "pch.h"
#include "Transform.h"
#include "EngineMath.h"

Transform::Transform()
{
	m_parent = nullptr;

	m_name = "transform";

	m_translate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotate = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_scale = XMFLOAT3(1.0f, 1.0f, 1.0f);

	m_local = EngineMath::Identity4x4();
	m_localScale = EngineMath::Identity4x4();
	m_localRotation = EngineMath::Identity4x4();
	m_localTranslation = EngineMath::Identity4x4();
}

Transform::~Transform()
{
}

void Transform::Init()
{
}

void Transform::Update()
{
}

void Transform::UpdateTranslate(const XMFLOAT3& translate)
{
	UpdateTranslate(translate.x, translate.x, translate.x);
}

void Transform::UpdateTranslate(const XMVECTOR& translate)
{
	float x = XMVectorGetX(translate);
	float y = XMVectorGetY(translate);
	float z = XMVectorGetZ(translate);

	UpdateTranslate(x, y, z);
}

void Transform::UpdateTranslate(float tx, float ty, float tz)
{
	float X = m_translate.x + tx;
	float Y	= m_translate.y + ty;
	float Z	= m_translate.z + tz;

	SetTranslate(X, Y, Z);
}

void Transform::UpdateRotate(const XMFLOAT3& rotate)
{
	UpdateRotate(rotate.x, rotate.y, rotate.z);
}

void Transform::UpdateRotate(const XMVECTOR& rotate)
{
	float x = XMVectorGetX(rotate);
	float y = XMVectorGetY(rotate);
	float z = XMVectorGetZ(rotate);

	UpdateRotate(x, y, z);
}

void Transform::UpdateRotate(float rx, float ry, float rz)
{
	float X = m_rotate.x + rx;
	float Y = m_rotate.y + ry;
	float Z = m_rotate.z + rz;

	SetRotate(X, Y, Z);
}

void Transform::UpdateScale(const XMFLOAT3& scale)
{
	UpdateScale(scale.x, scale.y, scale.z);
}

void Transform::UpdateScale(const XMVECTOR& scale)
{
	float x = XMVectorGetX(scale);
	float y = XMVectorGetY(scale);
	float z = XMVectorGetZ(scale);

	UpdateScale(x, y, z);
}

void Transform::UpdateScale(float sx, float sy, float sz)
{
	float X = m_scale.x + sx;
	float Y = m_scale.y + sy;
	float Z = m_scale.z + sz;

	SetScale(X, Y, Z);
}

void Transform::SetName(const string& name)
{
	m_name = name;
}

void Transform::SetTranslate(const XMFLOAT3& translate)
{
	SetTranslate(translate.x, translate.y, translate.z);
}

void Transform::SetTranslate(const XMVECTOR& translate)
{
	SetTranslate(XMVectorGetX(translate), XMVectorGetY(translate), XMVectorGetZ(translate));
}

void Transform::SetTranslate(float tx, float ty, float tz)
{
	//	Input : Degrees
	m_translate = { tx, ty, tz };

	XMStoreFloat4x4(&m_localTranslation, XMMatrixTranslation(tx, ty, tz));

	UpdateLocal();
}

void Transform::SetRotate(const XMFLOAT3& rotate)
{
	SetRotate(rotate.x, rotate.y, rotate.z);
}

void Transform::SetRotate(const XMVECTOR& rotate)
{
	SetRotate(XMVectorGetX(rotate), XMVectorGetY(rotate), XMVectorGetZ(rotate));
}

void Transform::SetRotate(float rx, float ry, float rz)
{
	m_rotate = { rx, ry, rz };

	XMMATRIX X = XMMatrixRotationX(EngineMath::DegreeToRadian(rx));
	XMMATRIX Y = XMMatrixRotationY(EngineMath::DegreeToRadian(ry));
	XMMATRIX Z = XMMatrixRotationZ(EngineMath::DegreeToRadian(rz));

	XMStoreFloat4x4(&m_localRotation, XMMatrixMultiply(XMMatrixMultiply(X, Y), Z));

	UpdateLocal();
}

void Transform::SetScale(const XMFLOAT3& scale)
{
	SetScale(scale.x, scale.y, scale.z);
}

void Transform::SetScale(const XMVECTOR& scale)
{
	SetScale(XMVectorGetX(scale), XMVectorGetY(scale), XMVectorGetZ(scale));
}

void Transform::SetScale(float sx, float sy, float sz)
{
	m_scale = { sx, sy, sz };

	XMStoreFloat4x4(&m_localScale, XMMatrixScaling(sx, sy, sz));

	UpdateLocal();
}

string Transform::GetName()
{
	return m_name;
}

XMFLOAT3 Transform::GetTranslate()
{
	return m_translate;
}

XMVECTOR Transform::GetTranslateAsVector()
{
	return XMLoadFloat3(&m_translate);
}

XMFLOAT3 Transform::GetRotate()
{
	return m_rotate;
}

XMVECTOR Transform::GetRotateAsVector()
{
	return XMLoadFloat3(&m_rotate);
}

XMFLOAT3 Transform::GetScale()
{
	return m_scale;
}

XMVECTOR Transform::GetScaleAsVector()
{
	return XMLoadFloat3(&m_scale);
}

XMFLOAT4X4 Transform::GetLocal()
{
	return m_local;
}

XMMATRIX Transform::GetWorld()
{
	UpdateLocal();

	XMMATRIX local = XMLoadFloat4x4(&m_local);

	if (!m_parent)
		return local;

	m_parent->UpdateLocal();
	XMMATRIX parent = XMLoadFloat4x4(&m_parent->GetLocal());
	XMMATRIX world = XMMatrixMultiply(local, parent);

	return world;
}

XMVECTOR Transform::GetRight()
{
	UpdateLocal();

	XMVECTOR vec = { m_local(0, 0), m_local(0, 1), m_local(0, 2) };

	return vec;
}

XMVECTOR Transform::GetUp()
{
	UpdateLocal();

	XMVECTOR vec = { m_local(1, 0), m_local(1, 1), m_local(1, 2) };

	return vec;
}

XMVECTOR Transform::GetLook()
{
	UpdateLocal();

	XMVECTOR vec = { m_local(2, 0), m_local(2, 1), m_local(2, 2) };

	return vec;
}

shared_ptr<Transform> Transform::GetParent()
{
	if (!m_parent)
		return nullptr;

	return m_parent;
}

void Transform::SetParent(shared_ptr<Transform> parent)
{
	m_parent = make_shared<Transform>();
	m_parent = parent;
}

void Transform::IndependantFromParent()
{
	if (!m_parent)
		return;

	m_parent = nullptr;
}

void Transform::UpdateLocal()
{
	XMMATRIX SR = XMMatrixMultiply(XMLoadFloat4x4(&m_localScale), XMLoadFloat4x4(&m_localRotation));
	XMMATRIX SRT = XMMatrixMultiply(SR, XMLoadFloat4x4(&m_localTranslation));

	XMStoreFloat4x4(&m_local, SRT);
}
