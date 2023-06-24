#pragma once
#include "EngineUtils.h"
#include "Device.h"
#include "CommandObjects.h"
#include "SwapChain.h"
#include "RootSignature.h"
#include "SRVDescriptorHeap.h"
#include "MultiRenderTarget.h"

class EngineSystem
{
	DECLARE_SINGLE(EngineSystem)

public:
	bool Init(WindowInfo wndInfo);
	void Update(ID3D12PipelineState* pPSO = nullptr);
	void Render(D3D12_VIEWPORT viewport, D3D12_RECT scissorRect);
	void PostUpdate();

public:
	void CreateDeferredRenderTargets(WindowInfo wndInfo);

	void ResetCommandAllocator();
	void ResetCommandList(ID3D12PipelineState* pPSO);
	void ResetBackBufferResourceState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after);

	void SetViewport(D3D12_VIEWPORT viewport);
	void SetScissorRect(D3D12_RECT scissorRect);
	void SetGraphicsRootSignature();

	void SetSRVDescriptorHeap(uint8 registerID, D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle);
	void SetSRVDescriptorHeaps();
	void CommitSRVDescriptorHeaps();

	void Present();

	void FlushCommandQueue();
	void CommitCommandLists();

public:
	shared_ptr<Device>					GetDEVICE()								{ return m_device; }
	shared_ptr<CommandObjects>			GetCOMMANDOBJECTS()						{ return m_cmdObjects; }
	shared_ptr<SwapChain>				GetSWAPCHAIN()							{ return m_swapChain; }
	shared_ptr<SRVDescriptorHeap>		GetSRVDESCRIPTORHEAP()					{ return m_srvDescriptorHeap; }

	ComPtr<ID3D12Device>				GetDevice()								{ return m_device->GetDevice(); }
	ComPtr<IDXGIFactory>				GetDXGI()								{ return m_device->GetDXGI(); }

	ComPtr<ID3D12CommandAllocator>		GetCmdAllocator()						{ return m_cmdObjects->GetCommandAllocator(); }
	ComPtr<ID3D12CommandQueue>			GetCmdQueue()							{ return m_cmdObjects->GetCommandQueue(); }
	ComPtr<ID3D12GraphicsCommandList>	GetCmdList()							{ return m_cmdObjects->GetCommandList(); }

	ComPtr<IDXGISwapChain>				GetSwapChain()							{ return m_swapChain->GetSwapChain(); }
	MultiSampleAntiAliasing				GetMultiSampling()						{ return m_swapChain->GetMultiSampling(); }

	ComPtr<ID3D12RootSignature>			GetRootSignature()						{ return m_rootSignature->GetRootSignature(); }

	ComPtr<ID3D12DescriptorHeap>		GetSRVDescriptorHeap()					{ return m_srvDescriptorHeap->GetDescriptorHeap(); }
	D3D12_CPU_DESCRIPTOR_HANDLE			GetSRVCpuHandleStart()					{ return m_srvDescriptorHeap->GetCpuHandleStart(); }
	D3D12_CPU_DESCRIPTOR_HANDLE			GetSRVCpuHandleStart(uint32 registerID) { return m_srvDescriptorHeap->GetCpuHandle(registerID); }

	shared_ptr<MultiRenderTarget>		GetMultiRenderTarget(RENDER_TARGET_TYPE type) { return m_multiRenderTargets[static_cast<uint32>(type)]; }

	WindowInfo							GetWindowInfo()							{ return m_windowInfo; }

private:
	WindowInfo						m_windowInfo;

	shared_ptr<Device>				m_device;
	shared_ptr<CommandObjects>		m_cmdObjects;
	shared_ptr<SwapChain>			m_swapChain;
	shared_ptr<RootSignature>		m_rootSignature;
	shared_ptr<SRVDescriptorHeap>	m_srvDescriptorHeap;

	array<shared_ptr<MultiRenderTarget>, RENDER_TARGET_TYPE_COUNT> m_multiRenderTargets;
};

