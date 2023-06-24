#include "pch.h"
#include "Material.h"
#include "EngineSystem.h"

Material::Material()
{
	m_diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_specular = { 0.01f, 0.01f, 0.01f };
	m_ambient = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_shininess = 1.0f;

	m_texturesOn = { 0 };

	m_attributes = make_unique<UploadBuffer<CBMaterial>>(EngineSystem::GetInst()->GetDevice().Get(), 1, true);
}

Material::~Material()
{
}

void Material::Update()
{
}

void Material::Render()
{
}

void Material::SetName(const string& name)
{
	m_name = name;
}

void Material::SetDiffuse(const XMFLOAT4& rgba)
{
	m_diffuse = rgba;
}

void Material::SetDiffuse(float r, float g, float b, float a)
{
	m_diffuse.x = r;
	m_diffuse.y = g;
	m_diffuse.z = b;
	m_diffuse.w = a;
}

void Material::SetSpecular(const XMFLOAT3& rgb)
{
	m_specular = rgb;
}

void Material::SetSpecular(float r, float g, float b)
{
	m_specular.x = r;
	m_specular.y = g;
	m_specular.z = b;
}

void Material::SetShininess(float value)
{
	m_shininess = value;
}

void Material::SetAmbient(const XMFLOAT4& rgba)
{
	m_ambient = rgba;
}

void Material::SetAmbient(float r, float g, float b, float a)
{
	m_ambient.x = r;
	m_ambient.y = g;
	m_ambient.z = b;
	m_ambient.w = a;
}

void Material::SetTexture(TREGISTER_TYPE type, shared_ptr<Texture> texture)
{
	m_textures[REGISTER_ID(type)] = texture;
	m_texturesOn[REGISTER_ID(type)] = 1;
}

void Material::SetTexture(uint8 registerID, shared_ptr<Texture> texture)
{
	m_textures[registerID] = texture;
	m_texturesOn[registerID] = 1;
}

void Material::SetUserDataInt(uint8 index, uint32 value)
{
	m_userDataInt[index] = value;
}

void Material::SetUserDataFloat(uint8 index, float value)
{
	m_userDataFloat[index] = value;
}

string Material::GetName()
{
	return m_name;
}

XMFLOAT4 Material::GetDiffuse()
{
	return m_diffuse;
}

XMFLOAT3 Material::GetSpecular()
{
	return m_specular;
}

XMFLOAT4 Material::GetAmbient()
{
	return m_ambient;
}

float Material::GetShininess()
{
	return m_shininess;
}

uint32 Material::GetUserDataInt(uint8 index)
{
	return m_userDataInt[index];
}

float Material::GetUserDataFloat(uint8 index)
{
	return m_userDataFloat[index];
}

void Material::UpdateAttributes()
{
	CBMaterial attributes;
	attributes.Diffuse = m_diffuse;
	attributes.Specular = m_specular;
	attributes.Shininess = m_shininess;
	attributes.Ambient = m_ambient;

	attributes.DiffuseTexture2D = m_texturesOn[REGISTER_ID(TREGISTER_TYPE::DIFFUSE_TEXTURE2D)];
	attributes.SpecularTexture2D = m_texturesOn[REGISTER_ID(TREGISTER_TYPE::SPECULAR_TEXTURE2D)];
	attributes.ShininessTexture2D = m_texturesOn[REGISTER_ID(TREGISTER_TYPE::SHININESS_TEXTURE2D)];
	attributes.NormalTexture2D = m_texturesOn[REGISTER_ID(TREGISTER_TYPE::NORMAL_TEXTURE2D)];

	attributes.UserDataInt_0 = m_userDataInt[0];
	attributes.UserDataInt_1 = m_userDataInt[1];
	attributes.UserDataInt_2 = m_userDataInt[2];
	attributes.UserDataInt_3 = m_userDataInt[3];

	attributes.UserDataFloat_0 = m_userDataFloat[0];
	attributes.UserDataFloat_1 = m_userDataFloat[1];
	attributes.UserDataFloat_2 = m_userDataFloat[2];
	attributes.UserDataFloat_3 = m_userDataFloat[3];

	attributes.UserDataMatrix_0 = m_userDataMatrix[0];
	attributes.UserDataMatrix_1 = m_userDataMatrix[1];
	attributes.UserDataMatrix_2 = m_userDataMatrix[2];
	attributes.UserDataMatrix_3 = m_userDataMatrix[3];

	m_attributes->CopyData(0, attributes);
}

void Material::RenderAttributes()
{
	RenderTextures();

	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBMaterial));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();

	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::MATERIAL), gpuAddress);
}

void Material::RenderTextures()
{
	D3D12_DESCRIPTOR_HEAP_DESC srvDescriptorHeap = {};
	srvDescriptorHeap.NumDescriptors = 4;
	srvDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	EngineSystem::GetInst()->GetDevice()->CreateDescriptorHeap(&srvDescriptorHeap, IID_PPV_ARGS(&m_descriptorHeap));

	m_cpuHandleSize = EngineSystem::GetInst()->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	for (size_t i = 0; i < m_textures.size(); ++i)
	{
		uint8 registerType = static_cast<uint8>(TREGISTER_TYPE::DIFFUSE_TEXTURE2D) + i;

		if (!m_texturesOn[i])
			continue;

		uint32 destRange = 1;
		uint32 srcRange = 1;

		CD3DX12_CPU_DESCRIPTOR_HANDLE destCpuHandle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
		destCpuHandle.Offset(i, m_cpuHandleSize);

		EngineSystem::GetInst()->GetDevice()->CopyDescriptors(1, &destCpuHandle, &destRange,
			1, &m_textures[i]->GetSRVCpuHandle(), &srcRange,
			D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { m_descriptorHeap.Get() };
	EngineSystem::GetInst()->GetCmdList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootDescriptorTable(REGISTER_ID(CREGISTER_TYPE::SRV_DESCRIPTOR_TABLE), srvGpuHandle);
}

//void Material::CommitTexturesToSRV(ComPtr<ID3D12Device> device)
//{
//	for (size_t i = 0; i < m_textures.size(); ++i)
//	{
//		uint8 registerType = static_cast<uint8>(TREGISTER_TYPE::DIFFUSE_TEXTURE2D) + i;
//
//		if (!m_texturesUsed[i])
//			continue;
//
//		EngineSystem::GetInst()->SetSRVDescriptorHeap(registerType, m_textures[i]->GetSRVCpuHandle());
//	}
//
//	EngineSystem::GetInst()->SetSRVDescriptorHeaps();
//	EngineSystem::GetInst()->CommitSRVDescriptorHeaps();
//}
