#pragma once
class SRVDescriptorHeap
{
public:
	SRVDescriptorHeap();
	~SRVDescriptorHeap();

public:
	bool Init(ComPtr<ID3D12Device> device, uint32 numDescriptors);

public:
	void SetDescriptorHeap(ComPtr<ID3D12Device> device, uint8 registerID,
						  D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle);
	void SetDescriptorHeaps(ComPtr<ID3D12GraphicsCommandList> cmdList);
	void CommitDescriptorHeaps(ComPtr<ID3D12GraphicsCommandList> cmdList);

public:
	ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap();

	D3D12_CPU_DESCRIPTOR_HANDLE  GetCpuHandle(uint8 registerID);
	D3D12_CPU_DESCRIPTOR_HANDLE  GetCpuHandleStart();

private:
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	uint32						 m_cpuHandleSize;
};