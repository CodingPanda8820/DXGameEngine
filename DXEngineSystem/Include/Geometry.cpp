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

void Geometry::Render(OBJECT_RENDER_TYPE type)
{
	GameObject::Render(type);

	switch (type)
	{
	case OBJECT_RENDER_TYPE::OBJECT:
		RenderObject();
		break;
	case OBJECT_RENDER_TYPE::SHADOW:
		RenderShadow();
		break;
	default:
		break;
	}
}

void Geometry::PostUpdate()
{
}

void Geometry::RenderObject()
{
	for (auto polySurface : m_polySurfaces)
	{
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

void Geometry::RenderShadow()
{
	for (auto polySurface : m_polySurfaces)
	{
		polySurface.second->Update();
		polySurface.second->Render();
	}
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
}

void Geometry::RenderAttributes()
{
}
