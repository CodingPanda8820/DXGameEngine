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

void GameObject::Render()
{
	if(m_shader)
		m_shader->Render();
}

void GameObject::PostUpdate()
{
}

void GameObject::SetShader(const string& shaderName)
{
	m_shader = ResourceManager::GetInst()->FindShader(shaderName);
}

shared_ptr<Shader> GameObject::GetShader()
{
	if (!m_shader)
		return nullptr;

	return m_shader;
}

void GameObject::UpdateAttributes()
{
	CBGameObject geoRenderStats;
	XMStoreFloat4x4(&geoRenderStats.World, XMMatrixTranspose(m_transform->GetWorld()));

	m_attributes->CopyData(0, geoRenderStats);
}

void GameObject::RenderAttributes()
{
	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBGameObject));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();

	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::GEOMETRY), gpuAddress);
}
