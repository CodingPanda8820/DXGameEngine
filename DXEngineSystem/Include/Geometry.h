#pragma once
#include "GameObject.h"

class Geometry : public GameObject
{
public:
	Geometry();
	virtual ~Geometry();

public:
	virtual void Init();
	virtual void Update();
	virtual void Render(OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);
	virtual void PostUpdate();

private:
	virtual void RenderObject();
	virtual void RenderShadow();

public:
	void SetVisibility(RENDER_LAYER layer, bool state);
	void AddPolySurface(const string& name);
	void AddPolySurfaces(vector<string> names);

	shared_ptr<Material>								GetMaterial();
	shared_ptr<PolySurface>								GetPolySurface(const string& name);
	unordered_map<string, shared_ptr<PolySurface>>		GetPolySurfaces();

private:
	array<bool, RENDER_LAYER_ID(RENDER_LAYER::COUNT)>	m_visibility;
	unordered_map<string, shared_ptr<PolySurface>>		m_polySurfaces;

public:
	virtual void UpdateAttributes();
	virtual void RenderAttributes();

public:
	unique_ptr<UploadBuffer<CBGameObject>> m_attributes;
};
