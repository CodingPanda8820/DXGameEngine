#pragma once
#include "EngineUtils.h"
#include "Device.h"
#include "CommandObjects.h"

class SwapChain
{
public:
	SwapChain();
	~SwapChain();

public:
	bool Init(const WindowInfo& wndInfo, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi,
			ComPtr<ID3D12GraphicsCommandList> cmdList, ComPtr<ID3D12CommandQueue> cmdQueue);
	void Update();
	void Render();
	void PostUpdate();

public:
	void Present();

public:
	ComPtr<IDXGISwapChain>			GetSwapChain();
	MultiSampleAntiAliasing			GetMultiSampling();
	int								GetCurrentBackBuffer();

private:
	bool InitMultiSampling(ComPtr<ID3D12Device> device);
	bool InitSwapChain(const WindowInfo& wndInfo, ComPtr<IDXGIFactory> dxgi, ComPtr<ID3D12CommandQueue> cmdQueue);

	void UpdateCurrentBackBufferIndex();

private:
	int	m_currentBackBuffer = 0;

	ComPtr<IDXGISwapChain>  m_swapChain;
	MultiSampleAntiAliasing m_multiSampling;
};

