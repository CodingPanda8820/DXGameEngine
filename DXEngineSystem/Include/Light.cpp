#include "pch.h"
#include "Light.h"
#include "EngineMath.h"
#include "ResourceManager.h"

Light::Light()
{
	m_material = make_shared<Material>();
	m_lightShape = make_shared<LightShape>();
	m_lightPoint = make_shared<PolySurface>();
	m_lightPoint = ResourceManager::GetInst()->FindPolySurface("PLY_LightGeometry");
}

Light::~Light()
{
}

void Light::Init(const string& name, LIGHT_TYPE type)
{
	GameObject::Init();

	SetName(name);
	SetLightType(type);
}

void Light::Update()
{
	GameObject::Update();
}

void Light::Render(OBJECT_RENDER_TYPE type)
{
	switch (type)
	{
	case OBJECT_RENDER_TYPE::OBJECT:
		GameObject::Render(type);
		RenderObject();
		break;
	case OBJECT_RENDER_TYPE::SHADOW:
		RenderShadow();
		break;
	default:
		break;
	}
}

void Light::RenderObject()
{
	UpdateLightMaterial();

	m_material->UpdateAttributes();
	m_material->RenderAttributes();

	m_lightPoint->Update();
	m_lightPoint->Render();
}

void Light::RenderShadow()
{
	UpdateLightMaterial(OBJECT_RENDER_TYPE::SHADOW);
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

void Light::SetLightType(LIGHT_TYPE type)
{
	m_lightType = type;
	switch (m_lightType)
	{
	case LIGHT_TYPE::AMBIENT:
	{
		SetShader("SHD_AmbientLight");
		SetMaterial("MAT_AmbientLight");
		break;
	}
	case LIGHT_TYPE::DIRECTIONAL:
	{
		SetShader("SHD_DirectionalLight");
		SetMaterial("MAT_DirectionalLight");
		break;
	}
	case LIGHT_TYPE::POINT:
	{
		SetShader("SHD_SpotLight");
		SetMaterial("MAT_PointLight");
		break;
	}
	case LIGHT_TYPE::SPOT:
	{
		SetShader("SHD_PointLight");
		SetMaterial("MAT_SpotLight");
		break;
	}
	default:
		break;
	}
}

void Light::SetLightIndex(uint32 index)
{
	m_lightIndex = index;
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

string Light::GetName()
{
	return m_name;
}

LIGHT_TYPE Light::GetLightType()
{
	return m_lightType;
}

shared_ptr<Material> Light::GetMaterial()
{
	return m_material;
}

shared_ptr<PolySurface> Light::GetVolumeShape()
{
	return m_lightPoint;
}

uint32 Light::GetLightIndex()
{
	return m_lightIndex;
}

XMFLOAT3 Light::GetStrength()
{
	return m_lightShape->GetStrength();
}

XMFLOAT3 Light::GetPosition()
{
	return m_transform->GetWorldPivotFloat3();
}

XMFLOAT3 Light::GetDirection()
{
	return m_transform->GetWorldYawFloat3();
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

void Light::UpdateLightMaterial(OBJECT_RENDER_TYPE type)
{
	shared_ptr<Material> material = make_shared<Material>();

	switch (type)
	{
	case OBJECT_RENDER_TYPE::OBJECT:
		material = m_material;
		break;
	case OBJECT_RENDER_TYPE::SHADOW:
		material = m_material_shadow;
		break;
	default:
		break;
	}

	material->SetUserDataInt(1, m_lightIndex);

	float fovY = 0.25 * EngineMath::Pi;

	float Width = EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::SHADOW)->GetRenderTarget(0)->GetTextureWidth();
	float Height = EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::SHADOW)->GetRenderTarget(0)->GetTextureHeight();

	XMFLOAT4X4 xformView = m_transform->GetWorldViewTransformFloat4x4();
	material->SetUserDataMatrix(0, xformView);

	XMFLOAT4X4 xformProjection = m_transform->GetWorldProjectionTransformFloat4x4(fovY, Width, Height, 0.001f, 1000.0f);
	material->SetUserDataMatrix(1, xformProjection);

	XMVECTOR xformViewDeterminant = XMMatrixDeterminant(XMLoadFloat4x4(&xformView));
	XMMATRIX xformViewInverse = XMMatrixInverse(&xformViewDeterminant, XMLoadFloat4x4(&xformView));
	XMStoreFloat4x4(&material->GetUserDataMatrix(2), xformViewInverse);

	XMVECTOR xformProjectionDeterminant = XMMatrixDeterminant(XMLoadFloat4x4(&xformProjection));
	XMMATRIX xformProjectionInverse = XMMatrixInverse(&xformProjectionDeterminant, XMLoadFloat4x4(&xformProjection));
	XMStoreFloat4x4(&material->GetUserDataMatrix(2), xformProjectionInverse);

	//XMFLOAT4 distributionVectors = Get
	EngineMath::GetDistributionVectors(material->GetUserDataFloat4sStart());
}

void Light::UpdateAttributes()
{
	CBLight attributes;
	attributes.Strength = GetStrength();
	attributes.FalloffStart = GetFalloffStart();
	attributes.Direction = GetDirection();
	attributes.FalloffEnd = GetFalloffEnd();
	attributes.Position = GetPosition();
	attributes.SpotPower = GetSpotPower();

	m_attributes->CopyData(0, attributes);
}

void Light::RenderAttributes()
{
	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBLight));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();
	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::LIGHT), gpuAddress);
}
