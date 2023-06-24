#include "pch.h"
#include "CameraShape.h"
#include "EngineUtils.h"
#include "EngineMath.h"
#include "EngineInput.h"

CameraShape::CameraShape()
{
	m_eyePosition = { 0.0f, 0.0f, 0.0f };
	m_fovY = 0.25	* EngineMath::Pi;
	m_resolutionW = 1280.0;
	m_resolutionH = 720.0;
	m_aspectRatio = m_resolutionW / m_resolutionH;
	m_nearClipping = 1.0f;
	m_farClipping = 1000.0f;
}

CameraShape::~CameraShape()
{
}

string CameraShape::GetName()
{
	return m_name;
}

XMFLOAT3 CameraShape::GetEyePosition()
{
	return m_eyePosition;
}

XMVECTOR CameraShape::GetEyePositionAsVector()
{
	return XMLoadFloat3(&m_eyePosition);
}

float CameraShape::GetFovX()
{
	return m_fovX;
}

float CameraShape::GetFovY()
{
	return m_fovY;
}

float CameraShape::GetResolutionW()
{
	return m_resolutionW;
}

float CameraShape::GetResolutionH()
{
	return m_resolutionH;
}

float CameraShape::GetAspectRatio()
{
	return m_aspectRatio;
}

float CameraShape::GetNearClipping()
{
	return m_nearClipping;
}

float CameraShape::GetFarClipping()
{
	return m_farClipping;
}

void CameraShape::SetName(const string& name)
{
	m_name = name;
}

void CameraShape::SetEyePosition(const XMFLOAT3& position)
{
	m_eyePosition = position;
}

void CameraShape::SetEyePosition(float x, float y, float z)
{
	m_eyePosition.x = x;
	m_eyePosition.y = y;
	m_eyePosition.z = z;
}

void CameraShape::SetFovY(float value)
{
	m_fovY = value;

	UpdateFrustrumInfo();
}

void CameraShape::SetResolution(float width, float height)
{
	m_resolutionW = width;
	m_resolutionH = height;

	UpdateAspectRatio();
}

void CameraShape::SetResolutionW(float value)
{
	m_resolutionW = value;

	UpdateAspectRatio();
}

void CameraShape::SetResolutionH(float value)
{
	m_resolutionH = value;

	UpdateAspectRatio();
}

void CameraShape::SetNearClipping(float value)
{
	m_nearClipping = value;

	UpdateFrustrumInfo();
}

void CameraShape::SetFarClipping(float value)
{
	m_farClipping = value;

	UpdateFrustrumInfo();
}

void CameraShape::UpdateAspectRatio()
{
	m_aspectRatio = m_resolutionW / m_resolutionH;

	UpdateFrustrumInfo();
}

void CameraShape::UpdateFrustrumInfo()
{
	m_nearWindowHeight = 2.0f * m_nearClipping * tanf(0.5f * m_fovY);
	m_nearWindowWidth = m_aspectRatio * m_nearWindowHeight;

	m_farWindowHeight = 2.0f * m_farClipping * tanf(0.5f * m_fovY);
	m_farWindowWidth = m_aspectRatio * m_farWindowHeight;

	m_fovX = 2.0f * atan((0.5f * m_nearWindowWidth) / m_nearClipping);
}
