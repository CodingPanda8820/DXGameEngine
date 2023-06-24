#include "pch.h"
#include "Geometry.h"
#include "EngineObjIO.h"
#include "EngineSystem.h"

Geometry::Geometry()
{
	m_shader = make_shared<Shader>();
	m_material = make_shared<Material>();

	m_visibility = { true };
}

Geometry::~Geometry()
{
}

void Geometry::Init()
{
	GameObject::Init();
}

void Geometry::Update()
{
	GameObject::Update();
}

void Geometry::Render()
{
	GameObject::Render();

	for (auto polySurface : m_polySurfaces)
	{
		UpdateAttributes();
		RenderAttributes();

		polySurface.second->Update();

		for (auto polySurfaceShape : polySurface.second->GetPolySurfaceShapes())
		{
			shared_ptr<Material> material = ResourceManager::GetInst()->FindMaterial(polySurfaceShape.second.MaterialName);

			if (material)
			{
				material->UpdateAttributes();
				material->RenderAttributes();
			}

			polySurface.second->RenderPolySurfaceShape(polySurfaceShape.first);
		}		
	}
}

void Geometry::PostUpdate()
{
}

void Geometry::SetMaterial(const string& name)
{
	m_material = ResourceManager::GetInst()->FindMaterial(name);
}

void Geometry::SetVisibility(RENDER_LAYER layer, bool state)
{
	m_visibility[RENDER_LAYER_ID(layer)] = state;
}

void Geometry::AddPolySurface(const string& name)
{
	m_polySurfaces[name] = make_shared<PolySurface>();
	m_polySurfaces[name] = ResourceManager::GetInst()->FindPolySurface(name);
}

void Geometry::AddPolySurfaces(vector<string> names)
{
	for (string name : names)
	{
		AddPolySurface(name);
	}
}

shared_ptr<Material> Geometry::GetMaterial()
{
	if (!m_material)
		return nullptr;

	return m_material;
}

shared_ptr<PolySurface> Geometry::GetPolySurface(const string& name)
{
	return m_polySurfaces[name];
}

unordered_map<string, shared_ptr<PolySurface>> Geometry::GetPolySurfaces()
{
	return m_polySurfaces;
}

void Geometry::UpdateAttributes()
{
	GameObject::UpdateAttributes();

	if(m_material)
		m_material->UpdateAttributes();
}

void Geometry::RenderAttributes()
{
	GameObject::RenderAttributes();

	if(m_material)
		m_material->RenderAttributes();
}
