#include "pch.h"
#include "Light.h"

Light::Light()
{
	m_material = make_shared<Material>();
	m_lightShape = make_shared<LightShape>();	
	m_volumeShape = make_shared<PolySurface>();
}

Light::~Light()
{
}

void Light::Init(const string& name, LIGHT_TYPE type)
{
	GameObject::Init();

	SetName(name);
	SetType(type);
}

void Light::Update()
{
	GameObject::Update();
}

void Light::Render()
{
	GameObject::Render();

	GameObject::UpdateAttributes();
	GameObject::RenderAttributes();

	m_material->UpdateAttributes();
	m_material->RenderAttributes();

	m_volumeShape->Update();
	m_volumeShape->Render();
}

void Light::PostUpdate()
{
	GameObject::PostUpdate();
}

void Light::SetName(const string& name)
{
	m_name = name;
	m_lightShape->SetName(name);
}

void Light::SetType(LIGHT_TYPE type)
{
	m_type = type;

	switch (m_type)
	{
	case LIGHT_TYPE::AMBIENT:
	{
		SetShader("SHD_AmbientLight");
		SetVolumeShape("EngineWindow");
		SetMaterial("MAT_AmbientLight");
		break;
	}
	case LIGHT_TYPE::DIRECTIONAL:
	{
		SetShader("SHD_DirectionalLight");
		SetVolumeShape("EngineWindow");
		SetMaterial("MAT_DirectionalLight");
		break;
	}
	case LIGHT_TYPE::POINT:
	{
		SetShader("SHD_SpotLight");
		SetVolumeShape("Sphere");
		SetMaterial("MAT_PointLight");
		break;
	}
	case LIGHT_TYPE::SPOT:
	{
		SetShader("SHD_PointLight");
		SetVolumeShape("Sphere");
		SetMaterial("MAT_SpotLight");
		break;
	}
	default:
		break;
	}
}

void Light::SetMaterial(const string& name)
{
	if (!m_material)
		m_material = make_shared<Material>();

	m_material = ResourceManager::GetInst()->FindMaterial(name);
}

void Light::SetVolumeShape(const string& name)
{
	if (!m_volumeShape)
		m_volumeShape = make_shared<PolySurface>();

	m_volumeShape = ResourceManager::GetInst()->FindPolySurface(name);
}

void Light::SetStrength(XMFLOAT3& value)
{
	m_lightShape->SetStrength(value);
}

void Light::SetStrength(float x, float y, float z)
{
	m_lightShape->SetStrength(x, y, z);
}

void Light::SetFalloffStart(float value)
{
	m_lightShape->SetFalloffStart(value);
}

void Light::SetFalloffEnd(float value)
{
	m_lightShape->SetFalloffEnd(value);
}

void Light::SetSpotPower(float value)
{
	m_lightShape->SetSpotPower(value);
}

void Light::SetPosition(XMFLOAT3& value)
{
	m_lightShape->SetPosition(value);
}

void Light::SetPosition(float x, float y, float z)
{
	m_lightShape->SetPosition(x, y, z);
}

void Light::SetDirection(XMFLOAT3& value)
{
	m_lightShape->SetDirection(value);
}

void Light::SetDirection(float x, float y, float z)
{
	m_lightShape->SetDirection(x, y, z);
}

string Light::GetName()
{
	return m_name;
}

LIGHT_TYPE Light::GetType()
{
	return m_type;
}

shared_ptr<Material> Light::GetMaterial()
{
	return m_material;
}

shared_ptr<PolySurface> Light::GetVolumeShape()
{
	return m_volumeShape;
}

void Light::UpdateAttributes()
{
	GameObject::UpdateAttributes();

	CBLight attributes;
	attributes.Strength = GetStrength();
	attributes.FalloffStart = GetFalloffStart();
	attributes.Direction = GetDirection();
	attributes.FalloffEnd = GetFalloffEnd();
	XMStoreFloat3(&attributes.Position, XMVector3Transform(XMLoadFloat3(&GetPosition()), m_transform->GetWorld()));
	attributes.SpotPower = GetSpotPower();

	m_attributes->CopyData(0, attributes);
}

void Light::RenderAttributes()
{
	GameObject::RenderAttributes();

	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBLight));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();
	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::LIGHT), gpuAddress);
}

XMFLOAT3 Light::GetStrength()
{
	return m_lightShape->GetStrength();
}

XMFLOAT3 Light::GetPosition()
{
	return m_lightShape->GetPosition();
}

XMFLOAT3 Light::GetDirection()
{
	return m_lightShape->GetDirection();
}

float Light::GetSpotPower()
{
	return m_lightShape->GetSpotPower();
}

float Light::GetFalloffStart()
{
	return m_lightShape->GetFalloffStart();
}

float Light::GetFalloffEnd()
{
	return m_lightShape->GetFalloffEnd();
}

shared_ptr<LightShape> Light::GetLightShape()
{
	return m_lightShape;
}
