#include "pch.h"
#include "SRVDescriptorHeap.h"
#include "EngineUtils.h"

SRVDescriptorHeap::SRVDescriptorHeap()
{
}

SRVDescriptorHeap::~SRVDescriptorHeap()
{
}

bool SRVDescriptorHeap::Init(ComPtr<ID3D12Device> device, uint32 numDescriptors)
{
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	//descriptorHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_descriptorHeap));

	m_cpuHandleSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	return true;
}

void SRVDescriptorHeap::SetDescriptorHeap(ComPtr<ID3D12Device> device, uint8 registerID, D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle)
{
	uint32						destRange = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE destHandle = GetCpuHandle(registerID);

	uint32						srcRange  = 1;
	D3D12_CPU_DESCRIPTOR_HANDLE srcHandle = srcCpuHandle;

	device->CopyDescriptors(1, &destHandle, &destRange, 1, &srcHandle, &srcRange,
							D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void SRVDescriptorHeap::SetDescriptorHeaps(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	ID3D12DescriptorHeap* srvDescriptorHeaps[] = { m_descriptorHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(srvDescriptorHeaps), srvDescriptorHeaps);
}

void SRVDescriptorHeap::CommitDescriptorHeaps(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	CD3DX12_GPU_DESCRIPTOR_HANDLE srvGpuHandle(m_descriptorHeap->GetGPUDescriptorHandleForHeapStart());
	cmdList->SetGraphicsRootDescriptorTable(REGISTER_ID(CREGISTER_TYPE::SRV_DESCRIPTOR_TABLE), srvGpuHandle);
}

ComPtr<ID3D12DescriptorHeap> SRVDescriptorHeap::GetDescriptorHeap()
{
	return m_descriptorHeap;
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVDescriptorHeap::GetCpuHandle(uint8 registerID)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE handle(m_descriptorHeap->GetCPUDescriptorHandleForHeapStart());
	handle.Offset(registerID, m_cpuHandleSize);

	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE SRVDescriptorHeap::GetCpuHandleStart()
{
	return m_descriptorHeap->GetCPUDescriptorHandleForHeapStart();
}
