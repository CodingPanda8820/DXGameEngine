#include "pch.h"
#include "Device.h"
#include "SwapChain.h"
#include "CommandObjects.h"
#include "EngineUtils.h"


SwapChain::SwapChain()
{
}

SwapChain::~SwapChain()
{

}

bool SwapChain::Init(const WindowInfo& wndInfo, ComPtr<ID3D12Device> device, ComPtr<IDXGIFactory> dxgi,
					ComPtr<ID3D12GraphicsCommandList> cmdList, ComPtr<ID3D12CommandQueue> cmdQueue)
{
	if (!InitMultiSampling(device))
		return false;

	if (!InitSwapChain(wndInfo, dxgi, cmdQueue))
		return false;

	return true;
}

void SwapChain::Update()
{
}

void SwapChain::Render()
{
}

void SwapChain::PostUpdate()
{

}

void SwapChain::Present()
{
	m_swapChain->Present(0, 0);

	UpdateCurrentBackBufferIndex();
}

ComPtr<IDXGISwapChain> SwapChain::GetSwapChain()
{
	return m_swapChain;
}

MultiSampleAntiAliasing SwapChain::GetMultiSampling()
{
	return m_multiSampling;
}

int SwapChain::GetCurrentBackBuffer()
{
	return m_currentBackBuffer;
}

void SwapChain::UpdateCurrentBackBufferIndex()
{
	m_currentBackBuffer = (m_currentBackBuffer + 1) % SWAP_CHAIN_BUFFER_COUNT;
}

bool SwapChain::InitSwapChain(const WindowInfo& wndInfo, ComPtr<IDXGIFactory> dxgi,
							ComPtr<ID3D12CommandQueue> cmdQueue)
{
	m_swapChain.Reset();

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	swapChainDesc.BufferDesc.Width = wndInfo.Width;
	swapChainDesc.BufferDesc.Height = wndInfo.Height;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.Format = BACK_BUFFER_FORMAT;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
	swapChainDesc.OutputWindow = wndInfo.hWnd;
	swapChainDesc.Windowed = wndInfo.WindowMode;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	dxgi->CreateSwapChain(cmdQueue.Get(), &swapChainDesc, m_swapChain.GetAddressOf());

	return true;
}

bool SwapChain::InitMultiSampling(ComPtr<ID3D12Device> device)
{
	//  Multi-Sampling
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels, sizeof(msQualityLevels));

	//  MSAA's QualityLevel must be more than 0 in everytime
	m_multiSampling.Quality = msQualityLevels.NumQualityLevels;
	assert(m_multiSampling.Quality > 0 && "Unexpected MSAA Quality Level.");

	return true;
}