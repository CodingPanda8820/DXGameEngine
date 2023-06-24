#include "pch.h"
#include "CommandObjects.h"
#include "Engine.h"
#include "Device.h"

CommandObjects::CommandObjects()
{

}

CommandObjects::~CommandObjects()
{
}

bool CommandObjects::Init(ComPtr<ID3D12Device> device)
{

	if (!InitFence(device))
		return false;

	if (!InitCommandQueue(device))
		return false;

	if (!InitCommandAllocator(device))
		return false;

	if (!InitGraphicsCommandList(device))
		return false;

	return true;
}

void CommandObjects::Update()
{
}

void CommandObjects::Render()
{
}

void CommandObjects::PostUpdate()
{
}

void CommandObjects::ResetCommandAllocator()
{
	m_cmdAllocator->Reset();
}

void CommandObjects::ResetCommandList(ID3D12PipelineState* pPSO)
{
	m_cmdList->Reset(m_cmdAllocator.Get(), pPSO);
}

bool CommandObjects::InitCommandQueue(ComPtr<ID3D12Device> device)
{
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc = {}; // Priority = 0; NodeMask = 0;
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue));

	return true;
}

bool CommandObjects::InitCommandAllocator(ComPtr<ID3D12Device> device)
{
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS( m_cmdAllocator.GetAddressOf()));

	return true;
}

bool CommandObjects::InitGraphicsCommandList(ComPtr<ID3D12Device> device)
{
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_cmdAllocator.Get(),
							nullptr, IID_PPV_ARGS(m_cmdList.GetAddressOf()));

	return true;
}

bool CommandObjects::InitFence(ComPtr<ID3D12Device> device)
{
	device->CreateFence(m_currentFence, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));

	return true;
}

void CommandObjects::FlushCommandQueue()
{
	m_currentFence++;

	m_cmdQueue->Signal(m_fence.Get(), m_currentFence);

	if (m_fence->GetCompletedValue() < m_currentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		m_fence->SetEventOnCompletion(m_currentFence, eventHandle);

		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}

void CommandObjects::CommitCommandLists()
{
	m_cmdList->Close();

	ID3D12CommandList* cmdLists[] = { m_cmdList.Get() };
	m_cmdQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}
