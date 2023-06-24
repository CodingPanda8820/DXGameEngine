#pragma once
#include "GPUBufferUtils.h"

class CameraShape
{
public:
	CameraShape();
	~CameraShape();

public:
	string	GetName();

	XMFLOAT3 GetEyePosition();
	XMVECTOR GetEyePositionAsVector();

	float	GetFovX();
	float	GetFovY();
	float	GetResolutionW();
	float	GetResolutionH();
	float	GetAspectRatio();
	float	GetNearClipping();
	float	GetFarClipping();

public:
	void	SetName(const string& name);

	void	SetEyePosition(const XMFLOAT3& position);
	void	SetEyePosition(float x, float y, float z);

	void	SetFovY(float value);
	void	SetResolution(float width, float height);
	void	SetResolutionW(float value);
	void	SetResolutionH(float value);
	void	SetNearClipping(float value);
	void	SetFarClipping(float value);

	void	UpdateAspectRatio();
	void	UpdateFrustrumInfo();

private:
	string	m_name;

	XMFLOAT3 m_eyePosition;

	float	m_fovY;
	float	m_fovX;

	float	m_resolutionW;
	float	m_resolutionH;
	float	m_aspectRatio;

	float	m_nearClipping;
	float	m_nearWindowWidth;
	float	m_nearWindowHeight;
	float	m_farClipping;
	float	m_farWindowWidth;
	float	m_farWindowHeight;
};

