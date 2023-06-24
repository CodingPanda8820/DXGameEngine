#pragma once
#include "GameObject.h"

class Geometry : public GameObject
{
public:
	Geometry();
	virtual ~Geometry();

public:
	virtual void Init() override;
	virtual void Update() override;
	virtual void Render() override;
	virtual void PostUpdate() override;

public:
	void SetMaterial(const string& name);
	void SetVisibility(RENDER_LAYER layer, bool state);
	void AddPolySurface(const string& name);
	void AddPolySurfaces(vector<string> names);

	shared_ptr<Material>								GetMaterial();
	shared_ptr<PolySurface>								GetPolySurface(const string& name);
	unordered_map<string, shared_ptr<PolySurface>>		GetPolySurfaces();

private:
	array<bool, RENDER_LAYER_ID(RENDER_LAYER::COUNT)>	m_visibility;

	shared_ptr<Material>								m_material;
	unordered_map<string, shared_ptr<PolySurface>>		m_polySurfaces;

public:
	virtual void UpdateAttributes() override;
	virtual void RenderAttributes() override;

public:
	unique_ptr<UploadBuffer<CBGameObject>> m_attributes;
};

