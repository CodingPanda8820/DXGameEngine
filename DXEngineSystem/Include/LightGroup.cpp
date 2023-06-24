#include "pch.h"
#include "LightGroup.h"

LightGroup::LightGroup()
{
}

LightGroup::~LightGroup()
{
}

void LightGroup::Init()
{
	GameObject::Init();

	m_attributes = make_unique<UploadBuffer<CBLightGroup>>(EngineSystem::GetInst()->GetDevice().Get(), 1, 0);
}

void LightGroup::Update()
{
	UpdateAttributes();
}

void LightGroup::Render(OBJECT_RENDER_TYPE type)
{
	RenderAttributes();
}

void LightGroup::PostUpdate()
{
}

void LightGroup::RenderObject()
{
}

void LightGroup::RenderShadow()
{
}

void LightGroup::AddLight(const string& name, LIGHT_TYPE type)
{
	m_lights[name] = make_shared<Light>();
	m_lights[name]->Init(name, type);

	switch (type)
	{
	case LIGHT_TYPE::AMBIENT:
		m_ambients.push_back(m_lights[name]);
		break;
	case LIGHT_TYPE::DIRECTIONAL:
		m_directionals.push_back(m_lights[name]);
		break;
	case LIGHT_TYPE::POINT:
		m_points.push_back(m_lights[name]);
		break;
	case LIGHT_TYPE::SPOT:
		m_spots.push_back(m_lights[name]);
		break;
	default:
		break;
	}
}

shared_ptr<Light> LightGroup::GetLight(const string& name)
{
	return m_lights[name];
}

unordered_map<string, shared_ptr<Light>> LightGroup::GetLights()
{
	return m_lights;
}

vector<shared_ptr<Light>> LightGroup::GetLights(LIGHT_TYPE type)
{
	switch (type)
	{
	case LIGHT_TYPE::AMBIENT:
		return m_ambients;
	case LIGHT_TYPE::DIRECTIONAL:
		return m_directionals;
	case LIGHT_TYPE::POINT:
		return m_points;
	case LIGHT_TYPE::SPOT:
		return m_spots;
	}
}

void LightGroup::UpdateAttributes()
{
	CBLightGroup attributes;

	int lightIndex = 0;

	for (auto light : m_lights)
	{
		light.second->SetLightIndex(lightIndex);
		light.second->GetMaterial()->SetUserDataInt(0, lightIndex);

		CBLight lightAttribute;
		lightAttribute.Strength = light.second->GetStrength();
		lightAttribute.FalloffStart = light.second->GetFalloffStart();
		lightAttribute.Direction = light.second->GetDirection();
		lightAttribute.FalloffEnd = light.second->GetFalloffEnd();
		lightAttribute.Position = light.second->GetPosition();
		lightAttribute.SpotPower = light.second->GetSpotPower();

		attributes.Light[lightIndex] = lightAttribute;

		lightIndex++;
	}

	m_attributes->CopyData(0, attributes);
}

void LightGroup::RenderAttributes()
{
	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBLightGroup));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();
	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::LIGHT), gpuAddress);
}
