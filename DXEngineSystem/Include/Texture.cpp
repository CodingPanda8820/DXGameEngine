#include "pch.h"
#include "Texture.h"

Texture::Texture()
{
}

Texture::~Texture()
{
}

void Texture::Create(ComPtr<ID3D12Device> device, DXGI_FORMAT format, uint32 width, uint32 height,
	const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
	D3D12_RESOURCE_FLAGS resourceFlags, XMFLOAT4 clearColor)
{
	D3D12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
	resourceDesc.Flags = resourceFlags;

	D3D12_CLEAR_VALUE optClearValue = {};
	D3D12_RESOURCE_STATES resourceStates = D3D12_RESOURCE_STATE_COMMON;

	if (resourceFlags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		resourceStates = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		optClearValue = CD3DX12_CLEAR_VALUE(DEPTH_STENCIL_FORMAT, 1.0f, 0);
	}
	else if (resourceFlags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
	{
		resourceStates = D3D12_RESOURCE_STATE_COMMON;
		float optClearColor[4] = { clearColor.x, clearColor.y, clearColor.z, clearColor.w };
		optClearValue = CD3DX12_CLEAR_VALUE(format, optClearColor);
	}

	HRESULT hr = device->CreateCommittedResource(&heapProperty, heapFlags, &resourceDesc, resourceStates,
		&optClearValue, IID_PPV_ARGS(&m_resource));

	assert(SUCCEEDED(hr));

	LoadGPUResource(device, m_resource);
}

void Texture::LoadGPUResource(ComPtr<ID3D12Device> device, ComPtr<ID3D12Resource> resource)
{
	m_resource = resource;
	D3D12_RESOURCE_DESC resourceDesc = resource->GetDesc();

	if (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
	{
		CreateDSV(device);
	}
	else
	{
		if (resourceDesc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
			CreateRTV(device);

		CreateSRV(device);
	}
}

void Texture::CreateSRV(ComPtr<ID3D12Device> device)
{
	D3D12_DESCRIPTOR_HEAP_DESC srvDescriptorHeap = {};
	srvDescriptorHeap.NumDescriptors = 1;
	srvDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	srvDescriptorHeap.NodeMask = 0;
	device->CreateDescriptorHeap(&srvDescriptorHeap, IID_PPV_ARGS(&m_srvDescriptorHeap));

	m_srvCpuHandle = m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Format = m_image.GetMetadata().format;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(m_resource.Get(), &srvDesc, m_srvCpuHandle);
}

void Texture::CreateRTV(ComPtr<ID3D12Device> device)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvDescriptorHeap = {};
	rtvDescriptorHeap.NumDescriptors = 1;
	rtvDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvDescriptorHeap.NodeMask = 0;
	device->CreateDescriptorHeap(&rtvDescriptorHeap, IID_PPV_ARGS(&m_rtvDescriptorHeap));

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateRenderTargetView(m_resource.Get(), nullptr, cpuHandle);
}

void Texture::CreateDSV(ComPtr<ID3D12Device> device)
{
	D3D12_DESCRIPTOR_HEAP_DESC dsvDescriptorHeap = {};
	dsvDescriptorHeap.NumDescriptors = 1;
	dsvDescriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDescriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvDescriptorHeap.NodeMask = 0;
	device->CreateDescriptorHeap(&dsvDescriptorHeap, IID_PPV_ARGS(&m_dsvDescriptorHeap));

	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	device->CreateDepthStencilView(m_resource.Get(), nullptr, cpuHandle);
}

void Texture::LoadTextureFile(const wstring& filePath, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	wstring extension = fileSystem::path(filePath).extension();

	if (extension == L".dds" || extension == L".DDS")
		LoadFromDDSFile(filePath.c_str(), DDS_FLAGS_NONE, nullptr, m_image);
	else if (extension == L".tga" || extension == L".TGA")
		LoadFromTGAFile(filePath.c_str(), nullptr, m_image);
	else
		LoadFromWICFile(filePath.c_str(), WIC_FLAGS_NONE, nullptr, m_image);

	HRESULT hr = CreateTexture(device.Get(), m_image.GetMetadata(), m_resource.GetAddressOf());
	if (FAILED(hr))
		assert(nullptr);

	vector<D3D12_SUBRESOURCE_DATA> subResources;
	hr = PrepareUpload(device.Get(), m_image.GetImages(), m_image.GetImageCount(), m_image.GetMetadata(), subResources);
	if (FAILED(hr))
		assert(nullptr);

	const uint64 bufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, static_cast<uint32>(subResources.size()));

	D3D12_HEAP_PROPERTIES uploaderHeapProperty = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	D3D12_RESOURCE_DESC uploaderDesc = CD3DX12_RESOURCE_DESC::Buffer(bufferSize);

	hr = device->CreateCommittedResource(&uploaderHeapProperty, D3D12_HEAP_FLAG_NONE, &uploaderDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(m_uploader.GetAddressOf()));

	if (FAILED(hr))
		assert(nullptr);

	UpdateSubresources(cmdList.Get(), m_resource.Get(), m_uploader.Get(), 0, 0,
		static_cast<uint32>(subResources.size()), subResources.data());

	CreateSRV(device);
}

void Texture::SetName(const string& name)
{
	m_name = name;
}

string Texture::GetName()
{
	return m_name;
}

float Texture::GetTextureWidth()
{
	return m_resource->GetDesc().Width;
}

float Texture::GetTextureHeight()
{
	return m_resource->GetDesc().Height;
}

ComPtr<ID3D12Resource> Texture::GetResource()
{
	return m_resource;
}

ComPtr<ID3D12DescriptorHeap> Texture::GetSRVDescriptorHeap()
{
	return m_srvDescriptorHeap;
}

ComPtr<ID3D12DescriptorHeap> Texture::GetRTVDescriptorHeap()
{
	return m_rtvDescriptorHeap;
}

ComPtr<ID3D12DescriptorHeap> Texture::GetDSVDescriptorHeap()
{
	return m_dsvDescriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetSRVCpuHandle()
{
	return m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
}
