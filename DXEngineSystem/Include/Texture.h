#pragma once
#include "EngineUtils.h"

class Texture
{
public:
	Texture();
	~Texture();

public:
	void Create(ComPtr<ID3D12Device> device, DXGI_FORMAT format, uint32 width, uint32 height,
				const D3D12_HEAP_PROPERTIES& heapProperty, D3D12_HEAP_FLAGS heapFlags,
				D3D12_RESOURCE_FLAGS resourceFlags, XMFLOAT4 clearColor = XMFLOAT4());

	void CreateSRV(ComPtr<ID3D12Device> device);
	void CreateRTV(ComPtr<ID3D12Device> device);
	void CreateDSV(ComPtr<ID3D12Device> device);

	void LoadGPUResource(ComPtr<ID3D12Device> device, ComPtr<ID3D12Resource> resource);

	void LoadTextureFile(const wstring& filePath, ComPtr<ID3D12Device> device,
						ComPtr<ID3D12GraphicsCommandList> cmdList);

public:
	void	SetName(const string& name);
	string	GetName();

public:
	ComPtr<ID3D12Resource>			GetResource();
	ComPtr<ID3D12DescriptorHeap>	GetSRVDescriptorHeap();
	ComPtr<ID3D12DescriptorHeap>	GetRTVDescriptorHeap();
	ComPtr<ID3D12DescriptorHeap>	GetDSVDescriptorHeap();

	D3D12_CPU_DESCRIPTOR_HANDLE		GetSRVCpuHandle();

private:
	string							m_name;

private:
	ScratchImage					m_image;
	ComPtr<ID3D12Resource>			m_resource;
	ComPtr<ID3D12Resource>			m_uploader;

	ComPtr<ID3D12DescriptorHeap>	m_srvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap>	m_rtvDescriptorHeap;
	ComPtr<ID3D12DescriptorHeap>	m_dsvDescriptorHeap;

	D3D12_CPU_DESCRIPTOR_HANDLE		m_srvCpuHandle;
};

