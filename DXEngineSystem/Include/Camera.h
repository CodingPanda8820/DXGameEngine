#pragma once
#include "GameObject.h"
#include "CameraShape.h"

enum class PROJECTION_TYPE
{
	PERSPECTIVE = 0,
	ORTHOGONAL,
};

class Camera : public GameObject
{
public:
	Camera(PROJECTION_TYPE type = PROJECTION_TYPE::PERSPECTIVE);
	virtual ~Camera();

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void PostUpdate() override;

public:
	virtual void UpdateAttributes() override;
	virtual void RenderAttributes() override;

public:
	void Dolly(float factor);		//  Translate	Y
	void Track(float factor);		//	Translate	X
	void Tilt(float radians);		//	Rotate Axis X
	void Panning(float radians);	//	Rotate Axis Y

public:
	shared_ptr<CameraShape> GetCameraShape();

	PROJECTION_TYPE GetProjectionType();

	XMFLOAT3	GetEyePosition();
	XMMATRIX	GetView();
	XMMATRIX	GetProjection();
	XMMATRIX	GetViewProjection();

	XMFLOAT4	GetFogColor();
	float		GetFogStart();
	float		GetFogEnd();

	void SetProjectionType(PROJECTION_TYPE type);

private:
	void UpdateViewMatrix();
	void UpdateProjectionMatrix();

private:
	PROJECTION_TYPE m_projectionType;

	XMMATRIX	m_view;
	XMMATRIX	m_projection;
	XMMATRIX	m_viewProjection;

	XMFLOAT4	m_fogColor;
	float		m_fogStart;
	float		m_fogEnd;

private:
	shared_ptr<CameraShape>				m_cameraShape;

private:
	unique_ptr<UploadBuffer<CBCamera>>	m_attributes;
};

