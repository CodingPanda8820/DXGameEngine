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

	m_localScale = EngineMath::Identity4x4();
	m_localRotation = EngineMath::Identity4x4();
	m_localTranslation = EngineMath::Identity4x4();
	m_localTransform = EngineMath::Identity4x4();
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
	UpdateTranslate(translate.x, translate.y, translate.z);
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
	float Y = m_translate.y + ty;
	float Z = m_translate.z + tz;

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

	UpdateLocalTransform();
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

	UpdateLocalTransform();
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

	UpdateLocalTransform();
}

string Transform::GetName()
{
	return m_name;
}

XMFLOAT3 Transform::GetLocalTranslateFloat3()
{
	return m_translate;
}

XMVECTOR Transform::GetLocalTranslateVector()
{
	return XMLoadFloat3(&GetLocalTranslateFloat3());
}

XMFLOAT3 Transform::GetLocalRotateFloat3()
{
	return m_rotate;
}

XMVECTOR Transform::GetLocalRotateVector()
{
	return XMLoadFloat3(&m_rotate);
}

XMFLOAT3 Transform::GetLocalScaleFloat3()
{
	return m_scale;
}

XMVECTOR Transform::GetLocalScaleVector()
{
	return XMLoadFloat3(&m_scale);
}

XMFLOAT4X4 Transform::GetLocalTransformFloat4x4()
{
	return m_localTransform;
}

XMFLOAT3 Transform::GetWorldRollFloat3()
{
	XMFLOAT4X4 world = GetWorldTransformFloat4x4();

	return XMFLOAT3(world._11, world._12, world._13);
}

XMVECTOR Transform::GetWorldRollVector()
{
	return XMLoadFloat3(&GetWorldRollFloat3());
}

XMFLOAT3 Transform::GetWorldPitchFloat3()
{
	XMFLOAT4X4 world = GetWorldTransformFloat4x4();

	return XMFLOAT3(world._21, world._22, world._23);
}

XMVECTOR Transform::GetWorldPitchVector()
{
	return XMLoadFloat3(&GetWorldPitchFloat3());
}

XMFLOAT3 Transform::GetWorldYawFloat3()
{
	XMFLOAT4X4 world = GetWorldTransformFloat4x4();

	return XMFLOAT3(world._31, world._32, world._33);
}

XMVECTOR Transform::GetWorldYawVector()
{
	return XMLoadFloat3(&GetWorldYawFloat3());
}

XMFLOAT3 Transform::GetWorldPivotFloat3()
{
	XMFLOAT4X4 world = GetWorldTransformFloat4x4();

	return XMFLOAT3(world._41, world._42, world._43);
}

XMVECTOR Transform::GetWorldPivotVector()
{
	return XMLoadFloat3(&GetWorldPivotFloat3());
}

XMMATRIX Transform::GetLocalTransformMatrix()
{
	return XMLoadFloat4x4(&GetLocalTransformFloat4x4());
}

XMFLOAT4X4 Transform::GetWorldTransformFloat4x4()
{
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, GetWorldTransformMatrix());

	return world;
}

XMMATRIX Transform::GetWorldTransformMatrix()
{
	UpdateLocalTransform();

	XMMATRIX local = XMLoadFloat4x4(&m_localTransform);

	if (!m_parent)
		return local;

	m_parent->UpdateLocalTransform();
	XMMATRIX parent = m_parent->GetWorldTransformMatrix();
	XMMATRIX world = XMMatrixMultiply(local, parent);

	return world;
}

XMFLOAT4X4 Transform::GetWorldViewTransformFloat4x4()
{
	XMFLOAT4X4 world = GetWorldTransformFloat4x4();

	XMVECTOR wRight = { world(0,0), world(0,1), world(0,2) };
	XMVECTOR wUp = { world(1,0), world(1,1), world(1,2) };
	XMVECTOR wDirection = { world(2,0), world(2,1), world(2,2) };
	XMVECTOR wPosition = { world(3,0), world(3,1), world(3,2) };

	wDirection = XMVector3Normalize(wDirection);
	wUp = XMVector3Normalize(XMVector3Cross(wDirection, wRight));
	wRight = XMVector3Cross(wUp, wDirection);

	float pr = -XMVectorGetX(XMVector3Dot(wPosition, wRight));
	float pu = -XMVectorGetX(XMVector3Dot(wPosition, wUp));
	float pl = -XMVectorGetX(XMVector3Dot(wPosition, wDirection));

	XMFLOAT4X4 view;
	view(0, 0) = XMVectorGetX(wRight);
	view(1, 0) = XMVectorGetY(wRight);
	view(2, 0) = XMVectorGetZ(wRight);
	view(3, 0) = pr;

	view(0, 1) = XMVectorGetX(wUp);
	view(1, 1) = XMVectorGetY(wUp);
	view(2, 1) = XMVectorGetZ(wUp);
	view(3, 1) = pu;

	view(0, 2) = XMVectorGetX(wDirection);
	view(1, 2) = XMVectorGetY(wDirection);
	view(2, 2) = XMVectorGetZ(wDirection);
	view(3, 2) = pl;

	view(0, 3) = 0.0f;
	view(1, 3) = 0.0f;
	view(2, 3) = 0.0f;
	view(3, 3) = 1.0f;

	return view;
}

XMMATRIX Transform::GetWorldViewTransformMatrix()
{
	return XMLoadFloat4x4(&GetWorldViewTransformFloat4x4());
}

XMFLOAT4X4 Transform::GetWorldProjectionTransformFloat4x4(float fovY, float resW, float resH, float nearZ, float farZ, PROJECTION_TYPE projectionType)
{
	XMFLOAT4X4 projection;
	XMStoreFloat4x4(&projection, GetWorldProjectionTransformMatrix(fovY, resW, resH, nearZ, farZ, projectionType));

	return projection;
}

XMMATRIX Transform::GetWorldProjectionTransformMatrix(float fovY, float resW, float resH, float nearZ, float farZ, PROJECTION_TYPE projectionType)
{
	float aspectRatio = resW / resH;

	switch (projectionType)
	{
	case PROJECTION_TYPE::PERSPECTIVE:
		return XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
	case PROJECTION_TYPE::ORTHOGONAL:
		return XMMatrixOrthographicLH(resW, resH, nearZ, farZ);
	default:
		return XMMATRIX();
	}
}

XMFLOAT3 Transform::GetLocalRollFloat3()
{
	UpdateLocalTransform();

	XMFLOAT3 vec = { m_localTransform(0, 0), m_localTransform(0, 1), m_localTransform(0, 2) };

	return vec;
}

XMVECTOR Transform::GetLocalRollVector()
{
	return XMLoadFloat3(&GetLocalRollFloat3());
}

XMFLOAT3 Transform::GetLocalPitchFloat3()
{
	UpdateLocalTransform();

	XMFLOAT3 vec = { m_localTransform(1, 0), m_localTransform(1, 1), m_localTransform(1, 2) };

	return vec;
}

XMVECTOR Transform::GetLocalPitchVector()
{
	return XMLoadFloat3(&GetLocalPitchFloat3());
}

XMFLOAT3 Transform::GetLocalYawFloat3()
{
	UpdateLocalTransform();

	XMFLOAT3 vec = { m_localTransform(2, 0), m_localTransform(2, 1), m_localTransform(2, 2) };

	return vec;
}

XMVECTOR Transform::GetLocalYawVector()
{
	return XMLoadFloat3(&GetLocalYawFloat3());
}

XMFLOAT3 Transform::GetLocalPivotFloat3()
{
	UpdateLocalTransform();

	XMFLOAT3 vec = { m_localTransform(3, 0), m_localTransform(3, 1), m_localTransform(3, 2) };

	return vec;
}

XMVECTOR Transform::GetLocalPivotVector()
{
	return XMLoadFloat3(&GetLocalPivotFloat3());
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

void Transform::UpdateLocalTransform()
{
	XMMATRIX SR = XMMatrixMultiply(XMLoadFloat4x4(&m_localScale), XMLoadFloat4x4(&m_localRotation));
	XMMATRIX SRT = XMMatrixMultiply(SR, XMLoadFloat4x4(&m_localTranslation));

	XMStoreFloat4x4(&m_localTransform, SRT);
}
