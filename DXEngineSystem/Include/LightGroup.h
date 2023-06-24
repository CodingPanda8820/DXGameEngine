#pragma once
#include "Light.h"

enum
{
	LIGHT_TYPE_COUNT = static_cast<uint8>(LIGHT_TYPE::COUNT),
};

class LightGroup : public GameObject
{
public:
	LightGroup();
	~LightGroup();

public:
	virtual void Init();
	virtual void Update();
	virtual void Render(OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);
	virtual void PostUpdate();

private:
	void RenderObject();
	void RenderShadow();

public:
	void AddLight(const string& name, LIGHT_TYPE type);

public:
	shared_ptr<Light>							GetLight(const string& name);
	vector<shared_ptr<Light>>					GetLights(LIGHT_TYPE type);
	unordered_map<string, shared_ptr<Light>>	GetLights();

private:
	void UpdateAttributes();
	void RenderAttributes();

private:
	unique_ptr<UploadBuffer<CBLightGroup>>	 m_attributes;

private:
	vector<shared_ptr<Light>>				 m_ambients;
	vector<shared_ptr<Light>>				 m_directionals;
	vector<shared_ptr<Light>>				 m_points;
	vector<shared_ptr<Light>>				 m_spots;

	unordered_map<string, shared_ptr<Light>> m_lights;
};

