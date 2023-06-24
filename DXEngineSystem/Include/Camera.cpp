#include "pch.h"
#include "Camera.h"

Camera::Camera(PROJECTION_TYPE type)
{
	m_cameraShape = make_shared<CameraShape>();
	m_projectionType = type;

	m_fogColor = { 0.8f, 0.8f, 0.8f, 1.0f };
	m_fogStart = 100.0f;
	m_fogEnd = 500.0f;
}

Camera::~Camera()
{
}

void Camera::Init()
{
	GameObject::Init();

	m_attributes = make_unique<UploadBuffer<CBCamera>>(EngineSystem::GetInst()->GetDevice().Get(), 1, true);
}

void Camera::Update()
{
	UpdateAttributes();
}

void Camera::Render()
{
	RenderAttributes();
}

void Camera::PostUpdate()
{
	GameObject::PostUpdate();
}

void Camera::Dolly(float factor)
{
	XMVECTOR translateFactor = XMVectorReplicate(factor);
	XMVECTOR translateVector = m_transform->GetWorldYawVector();
	XMVECTOR position = m_transform->GetWorldPivotVector();
	m_transform->SetTranslate(XMVectorMultiplyAdd(translateFactor, translateVector, position));
}

void Camera::Track(float factor)
{
	XMVECTOR translateFactor = XMVectorReplicate(factor);
	XMVECTOR translateVector = m_transform->GetWorldRollVector();
	XMVECTOR position = m_transform->GetWorldPivotVector();
	m_transform->SetTranslate(XMVectorMultiplyAdd(translateFactor, translateVector, position));
}

void Camera::Tilt(float radians)
{
	XMVECTOR input = m_transform->GetLocalRotateVector();
	XMVECTOR output = { XMVectorGetX(input) + radians, XMVectorGetY(input), XMVectorGetZ(input) };

	m_transform->SetRotate(output);
}

void Camera::Panning(float radians)
{
	XMVECTOR input = m_transform->GetLocalRotateVector();
	XMVECTOR output = { XMVectorGetX(input), XMVectorGetY(input) + radians, XMVectorGetZ(input) };

	m_transform->SetRotate(output);
}

shared_ptr<CameraShape> Camera::GetCameraShape()
{
	return m_cameraShape;
}

PROJECTION_TYPE Camera::GetProjectionType()
{
	return m_projectionType;
}

XMFLOAT3 Camera::GetEyePosition()
{
	return m_cameraShape->GetEyePosition();
}

XMMATRIX Camera::GetView()
{
	return m_view;
}

XMMATRIX Camera::GetProjection()
{
	return m_projection;
}

XMMATRIX Camera::GetViewProjection()
{
	return m_viewProjection;
}

XMFLOAT4 Camera::GetFogColor()
{
	return m_fogColor;
}

float Camera::GetFogStart()
{
	return m_fogStart;
}

float Camera::GetFogEnd()
{
	return m_fogEnd;
}

void Camera::SetProjectionType(PROJECTION_TYPE type)
{
	m_projectionType = type;
}

void Camera::UpdateAttributes()
{
	UpdateViewMatrix();
	UpdateProjectionMatrix();

	CBCamera attributes;
	XMStoreFloat3(&attributes.EyePosition, m_transform->GetWorldPivotVector());
	XMStoreFloat4x4(&attributes.View, XMMatrixTranspose(m_view));
	XMStoreFloat4x4(&attributes.ViewInv, XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(m_view), m_view)));
	XMStoreFloat4x4(&attributes.Projection, XMMatrixTranspose(m_projection));
	XMStoreFloat4x4(&attributes.ProjectionInv, XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(m_projection), m_projection)));

	XMMATRIX viewProjection = XMMatrixMultiply(m_view, m_projection);
	XMStoreFloat4x4(&attributes.ViewProjection, XMMatrixTranspose(viewProjection));
	XMStoreFloat4x4(&attributes.ViewProjectionInv, XMMatrixTranspose(XMMatrixInverse(&XMMatrixDeterminant(viewProjection), viewProjection)));

	attributes.FogColor = m_fogColor;
	attributes.FogStart = m_fogStart;
	attributes.FogEnd = m_fogEnd;

	m_attributes->CopyData(0, attributes);
}

void Camera::RenderAttributes()
{
	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBCamera));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();
	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::CAMERA), gpuAddress);
}

void Camera::UpdateViewMatrix()
{
	m_view = m_transform->GetWorldViewTransformMatrix();
}

void Camera::UpdateProjectionMatrix()
{
	float fovY			= m_cameraShape->GetFovY();
	float resW			= m_cameraShape->GetResolutionW();
	float resH			= m_cameraShape->GetResolutionH();
	float aspectRatio	= m_cameraShape->GetAspectRatio();
	float nearClipping	= m_cameraShape->GetNearClipping();
	float farClipping	= m_cameraShape->GetFarClipping();

	m_projection = m_transform->GetWorldProjectionTransformMatrix(fovY, resW, resH, nearClipping, farClipping, m_projectionType);
}