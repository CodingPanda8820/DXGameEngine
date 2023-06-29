#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	m_transform = make_shared<Transform>();
	m_attributes = make_unique<UploadBuffer<CBGameObject>>(EngineSystem::GetInst()->GetDevice().Get(), 1, 0);
}

GameObject::~GameObject()
{
}

void GameObject::Init()
{
}

void GameObject::Update()
{
}

void GameObject::Render(OBJECT_RENDER_TYPE type)
{
	UpdateAttributes();
	RenderAttributes();

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

void GameObject::PostUpdate()
{
}

void GameObject::RenderObject()
{
	if (m_shader)
		m_shader->Render();

	if (m_material)
	{
		m_material->UpdateAttributes();
		m_material->RenderAttributes();
	}
}

void GameObject::RenderShadow()
{
	if (m_shader_shadow)
		m_shader_shadow->Render();

	if (m_material_shadow)
	{
		m_material_shadow->UpdateAttributes();
		m_material_shadow->RenderAttributes();
	}
}

void GameObject::SetShader(const string & shaderName, OBJECT_RENDER_TYPE type)
{
	switch (type)
	{
	case OBJECT_RENDER_TYPE::OBJECT:
		m_shader = ResourceManager::GetInst()->FindShader(shaderName);
		break;
	case OBJECT_RENDER_TYPE::SHADOW:
		m_shader_shadow = ResourceManager::GetInst()->FindShader(shaderName);
		break;
	default:
		break;
	}
}

void GameObject::SetMaterial(const string & materialName, OBJECT_RENDER_TYPE type)
{
	switch (type)
	{
	case OBJECT_RENDER_TYPE::OBJECT:
		m_material = ResourceManager::GetInst()->FindMaterial(materialName);
		break;
	case OBJECT_RENDER_TYPE::SHADOW:
		m_material_shadow = ResourceManager::GetInst()->FindMaterial(materialName);
		break;
	default:
		break;
	}
}

void GameObject::SetScale(const XMFLOAT3 & scale)
{
	m_transform->SetScale(scale);
}

void GameObject::SetScale(const XMVECTOR & scale)
{
	m_transform->SetScale(scale);
}

void GameObject::SetScale(float sx, float sy, float sz)
{
	m_transform->SetScale(sx, sy, sz);
}

void GameObject::SetRotate(const XMFLOAT3 & rotate)
{
	m_transform->SetRotate(rotate);
}

void GameObject::SetRotate(const XMVECTOR & rotate)
{
	m_transform->SetRotate(rotate);
}

void GameObject::SetRotate(float rx, float ry, float rz)
{
	m_transform->SetRotate(rx, ry, rz);
}

void GameObject::SetTranslate(const XMFLOAT3 & translate)
{
	m_transform->SetTranslate(translate);
}

void GameObject::SetTranslate(const XMVECTOR & translate)
{
	m_transform->SetTranslate(translate);
}

void GameObject::SetTranslate(float tx, float ty, float tz)
{
	m_transform->SetTranslate(tx, ty, tz);
}

shared_ptr<Shader> GameObject::GetShader()
{
	if (!m_shader)
		return nullptr;

	return m_shader;
}

shared_ptr<Transform> GameObject::GetTransform()
{
	return m_transform;
}

void GameObject::UpdateAttributes()
{
	CBGameObject geoRenderStats;
	XMStoreFloat4x4(&geoRenderStats.World, XMMatrixTranspose(m_transform->GetWorldTransformMatrix()));

	m_attributes->CopyData(0, geoRenderStats);
}

void GameObject::RenderAttributes()
{
	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBGameObject));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();

	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::GEOMETRY), gpuAddress);
}
