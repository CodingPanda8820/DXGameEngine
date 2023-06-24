#pragma once
#include "EngineInput.h"
#include "EngineTimer.h"
#include "Camera.h"
#include "Geometry.h"
#include "LightGroup.h"

class Scene
{
public:
	Scene();
	~Scene();

public:
	bool Init();
	void Update(const float& deltaTime);
	void Render(const float& deltaTime);
	void PostUpdate(const float& deltaTime);

private:
	void InitCamera();
	void InitLightGroup();
	void InitGameUI();
	void InitGeometries();

private:
	void RenderDeferred();
	void RenderLighting();
	void RenderForward();
	void RenderGameUI();

private:
	void SetCameraToObserver(const string& name);
	void SetCameraToRenderGameUI(const string& name);

private:
	unordered_map<string, shared_ptr<Camera>>		m_cameras;

	shared_ptr<Camera>								m_observer;
	shared_ptr<LightGroup>							m_lightGroup;
	unordered_map<string, shared_ptr<Geometry>>		m_geometries;

	shared_ptr<Camera>								m_camera_gameUI;
	vector<shared_ptr<Geometry>>					m_renderLayers_gameUI;

	array<vector<shared_ptr<Geometry>>, RENDER_LAYER_ID(RENDER_LAYER::COUNT)> m_renderLayers_forward;
	array<vector<shared_ptr<Geometry>>, RENDER_LAYER_ID(RENDER_LAYER::COUNT)> m_renderLayers_deferred;

private:
	void UpdateInputAndTimer(const float& deltaTime);
	void RenderInputAndTimer(const float& deltaTime);

private:
	void UpdateTimer(const float& deltaTime);
	void RenderTimer();

	void UpdateMouseAction(const float& deltaTime);
	void UpdateKeyboardAction(const float& deltaTime);

private:
	float	m_totalTime;

	POINT	m_mousePosPrev;
	POINT	m_mouseReception;

	unique_ptr<UploadBuffer<CBScene>> m_attributes;
};

