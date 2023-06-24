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
	XMVECTOR translateVector = m_transform->GetLook();
	XMVECTOR position = m_transform->GetTranslateAsVector();
	m_transform->SetTranslate(XMVectorMultiplyAdd(translateFactor, translateVector, position));
}

void Camera::Track(float factor)
{
	XMVECTOR translateFactor = XMVectorReplicate(factor);
	XMVECTOR translateVector = m_transform->GetRight();
	XMVECTOR position = m_transform->GetTranslateAsVector();
	m_transform->SetTranslate(XMVectorMultiplyAdd(translateFactor, translateVector, position));
}

void Camera::Tilt(float radians)
{
	XMVECTOR input = m_transform->GetRotateAsVector();
	XMVECTOR output = { XMVectorGetX(input) + radians, XMVectorGetY(input), XMVectorGetZ(input) };

	m_transform->SetRotate(output);
}

void Camera::Panning(float radians)
{
	XMVECTOR input = m_transform->GetRotateAsVector();
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
	XMStoreFloat3(&attributes.EyePosition, XMVector3Transform(m_cameraShape->GetEyePositionAsVector(), m_transform->GetWorld()));
	XMStoreFloat4x4(&attributes.View, XMMatrixTranspose(m_view));
	XMStoreFloat4x4(&attributes.Projection, XMMatrixTranspose(m_projection));
	XMStoreFloat4x4(&attributes.ViewProjection, XMMatrixMultiplyTranspose(m_view, m_projection));
	attributes.FogColor = m_fogColor;
	attributes.FogStart = m_fogStart;
	attributes.FogEnd	= m_fogEnd;

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
	XMFLOAT4X4 world;
	XMStoreFloat4x4(&world, m_transform->GetWorld());

	XMVECTOR inheritPos = { world(3, 0), world(3, 1), world(3, 2) };
	XMVECTOR localPos = m_cameraShape->GetEyePositionAsVector();

	XMVECTOR worldPos	= XMVectorAdd(inheritPos, localPos);
	XMVECTOR right		= { world(0, 0), world(0, 1), world(0, 2) };
	XMVECTOR up			= { world(1, 0), world(1, 1), world(1, 2) };
	XMVECTOR look		= { world(2, 0), world(2, 1), world(2, 2) };

	look	= XMVector3Normalize(look);
	up		= XMVector3Normalize(XMVector3Cross(look, right));
	right	= XMVector3Cross(up, look);

	float pr = -XMVectorGetX(XMVector3Dot(worldPos, right));
	float pu = -XMVectorGetX(XMVector3Dot(worldPos, up));
	float pl = -XMVectorGetX(XMVector3Dot(worldPos, look));

	XMFLOAT4X4 view;
	view(0, 0) = XMVectorGetX(right);
	view(1, 0) = XMVectorGetY(right);
	view(2, 0) = XMVectorGetZ(right);
	view(3, 0) = pr;

	view(0, 1) = XMVectorGetX(up);
	view(1, 1) = XMVectorGetY(up);
	view(2, 1) = XMVectorGetZ(up);
	view(3, 1) = pu;

	view(0, 2) = XMVectorGetX(look);
	view(1, 2) = XMVectorGetY(look);
	view(2, 2) = XMVectorGetZ(look);
	view(3, 2) = pl;

	view(0, 3) = 0.0f;
	view(1, 3) = 0.0f;
	view(2, 3) = 0.0f;
	view(3, 3) = 1.0f;

	m_view = XMLoadFloat4x4(&view);
}

void Camera::UpdateProjectionMatrix()
{
	float fovY			= m_cameraShape->GetFovY();
	float resW			= m_cameraShape->GetResolutionW();
	float resH			= m_cameraShape->GetResolutionH();
	float aspectRatio	= m_cameraShape->GetAspectRatio();
	float nearClipping	= m_cameraShape->GetNearClipping();
	float farClipping	= m_cameraShape->GetFarClipping();

	switch (m_projectionType)
	{
	case PROJECTION_TYPE::PERSPECTIVE:
	{
		m_projection = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearClipping, farClipping);
		break;
	}
	case PROJECTION_TYPE::ORTHOGONAL:
	{
		m_projection = XMMatrixOrthographicLH(resW, resH, nearClipping, farClipping);
		break;
	}
	default:
		break;
	}
}