#pragma once
class CommandObjects
{
public:
	CommandObjects();
	~CommandObjects();

public:
	bool Init(ComPtr<ID3D12Device> device);
	void Update();
	void Render();
	void PostUpdate();

	void ResetCommandAllocator();
	void ResetCommandList(ID3D12PipelineState* pPSO = nullptr);

	void FlushCommandQueue();
	void CommitCommandLists();

public:
	ComPtr<ID3D12CommandQueue>			GetCommandQueue() { return m_cmdQueue; }
	ComPtr<ID3D12CommandAllocator>		GetCommandAllocator() { return m_cmdAllocator; }
	ComPtr<ID3D12GraphicsCommandList>	GetCommandList() { return m_cmdList; }

private:
	bool InitCommandQueue(ComPtr<ID3D12Device> device);
	bool InitCommandAllocator(ComPtr<ID3D12Device> device);
	bool InitGraphicsCommandList(ComPtr<ID3D12Device> device);
	bool InitFence(ComPtr<ID3D12Device> device);

	ComPtr<ID3D12CommandQueue>			m_cmdQueue;
	ComPtr<ID3D12CommandAllocator>		m_cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>	m_cmdList;

	UINT64					m_currentFence;
	ComPtr<ID3D12Fence>		m_fence;
};

