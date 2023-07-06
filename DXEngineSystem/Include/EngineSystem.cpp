#include "pch.h"
#include "EngineSystem.h"
#include "ResourceManager.h"

DEFINITION_SINGLE(EngineSystem)

EngineSystem::EngineSystem()
{
	m_device = make_shared<Device>();
	m_cmdObjects = make_shared<CommandObjects>();
	m_swapChain = make_shared<SwapChain>();
	m_rootSignature = make_shared<RootSignature>();
	m_srvDescriptorHeap = make_shared<SRVDescriptorHeap>();
}

EngineSystem::~EngineSystem()
{

}

bool EngineSystem::Init(WindowInfo wndInfo)
{
	m_windowInfo = wndInfo;

	if (!m_device->Init())
		return false;

	if (!m_cmdObjects->Init(GetDevice()))
		return false;

	if (!m_swapChain->Init(wndInfo, GetDevice(), GetDXGI(), GetCmdList(), GetCmdQueue()))
		return false;

	if (!m_rootSignature->Init(GetDevice()))
		return false;

	if (!m_srvDescriptorHeap->Init(GetDevice(), REGISTER_ID(TREGISTER_TYPE::COUNT)))
		return false;

	CreateDeferredRenderTargets(wndInfo);

	return true;
}

void EngineSystem::Update(ID3D12PipelineState* pPSO)
{
	ResetCommandAllocator();
	ResetCommandList(pPSO);

	ResetBackBufferResourceState(D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	SetGraphicsRootSignature();
}

void EngineSystem::Render(D3D12_VIEWPORT viewport, D3D12_RECT scissorRect)
{
	//SetViewport(viewport);
	//SetScissorRect(scissorRect);
}

void EngineSystem::PostUpdate()
{
	ResetBackBufferResourceState(D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	CommitCommandLists();

	Present();

	FlushCommandQueue();
}

void EngineSystem::SetViewport(D3D12_VIEWPORT viewport)
{
	GetCmdList()->RSSetViewports(1, &viewport);
}

void EngineSystem::SetScissorRect(D3D12_RECT scissorRect)
{
	GetCmdList()->RSSetScissorRects(1, &scissorRect);
}

void EngineSystem::SetGraphicsRootSignature()
{
	GetCmdList()->SetGraphicsRootSignature(GetRootSignature().Get());
}

void EngineSystem::SetSRVDescriptorHeap(uint8 registerID, D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle)
{
	m_srvDescriptorHeap->SetDescriptorHeap(GetDevice(), registerID, srcCpuHandle);
}

void EngineSystem::SetSRVDescriptorHeaps()
{
	m_srvDescriptorHeap->SetDescriptorHeaps(GetCmdList());
}

void EngineSystem::CommitSRVDescriptorHeaps()
{
	m_srvDescriptorHeap->CommitDescriptorHeaps(GetCmdList());
}

void EngineSystem::CreateDeferredRenderTargets(WindowInfo wndInfo)
{
	//	DepthStencil
	ResourceManager::GetInst()->NewTexture("DRT_DepthStencil", GetDevice(), DEPTH_STENCIL_FORMAT,
		wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	shared_ptr<Texture> dsTexture = ResourceManager::GetInst()->FindTexture("DRT_DepthStencil");

	//	SwapChain
	{
		vector<RenderTarget> rtSwapChain(SWAP_CHAIN_BUFFER_COUNT);
		for (uint32 i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
		{
			string name = "DRT_SwapChain_" + to_string(i);

			ComPtr<ID3D12Resource> resource;
			GetSwapChain()->GetBuffer(i, IID_PPV_ARGS(&resource));

			ResourceManager::GetInst()->NewTexture(name, GetDevice(), resource);
			rtSwapChain[i].Texture = ResourceManager::GetInst()->FindTexture(name);
		}

		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)] = make_shared<MultiRenderTarget>();
		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::SWAP_CHAIN)]->Create(GetDevice(), RENDER_TARGET_TYPE::SWAP_CHAIN, rtSwapChain, dsTexture);
	}

	//	Shadow
	{
		const float shadowQuality = 4;

		vector<RenderTarget> rtShadow(RENDER_TARGET_SHADOW_COUNT);
		ResourceManager::GetInst()->NewTexture("DRT_Shadow", GetDevice(), DXGI_FORMAT_R32_FLOAT,
			wndInfo.Width * shadowQuality, wndInfo.Height * shadowQuality,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtShadow[0].Texture = ResourceManager::GetInst()->FindTexture("DRT_Shadow");

		ResourceManager::GetInst()->NewTexture("DRT_ShadowDepth", GetDevice(), DEPTH_STENCIL_FORMAT,
			wndInfo.Width * shadowQuality, wndInfo.Height * shadowQuality,
			CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

		shared_ptr<Texture> shadowDepth = ResourceManager::GetInst()->FindTexture("DRT_ShadowDepth");

		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)] = make_shared<MultiRenderTarget>();
		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::SHADOW)]->Create(GetDevice(), RENDER_TARGET_TYPE::SHADOW, rtShadow, shadowDepth);
	}

	//	GBuffer
	{
		vector<RenderTarget> rtGBuffer(RENDER_TARGET_GBUFFER_COUNT);

		ResourceManager::GetInst()->NewTexture("DRT_Position", GetDevice(), DXGI_FORMAT_R32G32B32A32_FLOAT,
			wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtGBuffer[0].Texture = ResourceManager::GetInst()->FindTexture("DRT_Position");

		ResourceManager::GetInst()->NewTexture("DRT_DiffuseAndSpecular", GetDevice(), DXGI_FORMAT_R32G32B32A32_FLOAT,
			wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtGBuffer[1].Texture = ResourceManager::GetInst()->FindTexture("DRT_DiffuseAndSpecular");

		ResourceManager::GetInst()->NewTexture("DRT_NormalAndShininess", GetDevice(), DXGI_FORMAT_R32G32B32A32_FLOAT,
			wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtGBuffer[2].Texture = ResourceManager::GetInst()->FindTexture("DRT_NormalAndShininess");

		ResourceManager::GetInst()->NewTexture("DRT_Depth", GetDevice(), DXGI_FORMAT_R32G32B32A32_FLOAT,
			wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtGBuffer[3].Texture = ResourceManager::GetInst()->FindTexture("DRT_Depth");

		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::GBUFFER)] = make_shared<MultiRenderTarget>();
		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::GBUFFER)]->Create(GetDevice(), RENDER_TARGET_TYPE::GBUFFER, rtGBuffer, dsTexture);
	}

	//	Lighting
	{
		vector<RenderTarget> rtLighting(RENDER_TARGET_LIGHTING_COUNT);

		ResourceManager::GetInst()->NewTexture("DRT_DiffuseLight", GetDevice(), DXGI_FORMAT_R8G8B8A8_UNORM,
			wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtLighting[0].Texture = ResourceManager::GetInst()->FindTexture("DRT_DiffuseLight");

		ResourceManager::GetInst()->NewTexture("DRT_SpecularLight", GetDevice(), DXGI_FORMAT_R8G8B8A8_UNORM,
			wndInfo.Width, wndInfo.Height, CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
		rtLighting[1].Texture = ResourceManager::GetInst()->FindTexture("DRT_SpecularLight");

		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)] = make_shared<MultiRenderTarget>();
		m_multiRenderTargets[static_cast<uint8>(RENDER_TARGET_TYPE::LIGHTING)]->Create(GetDevice(), RENDER_TARGET_TYPE::LIGHTING, rtLighting, dsTexture);
	}
}

void EngineSystem::ResetCommandAllocator()
{
	m_cmdObjects->ResetCommandAllocator();
}

void EngineSystem::ResetCommandList(ID3D12PipelineState* pPSO)
{
	m_cmdObjects->ResetCommandList(pPSO);
}

void EngineSystem::ResetBackBufferResourceState(D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
{
	int currentBackBufferIndex = m_swapChain->GetCurrentBackBuffer();

	D3D12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		GetMultiRenderTarget(RENDER_TARGET_TYPE::SWAP_CHAIN)->GetRenderTarget(currentBackBufferIndex)->GetResource().Get(), before, after);

	GetCmdList()->ResourceBarrier(1, &barrier);
}

void EngineSystem::Present()
{
	m_swapChain->Present();
}

void EngineSystem::FlushCommandQueue()
{
	m_cmdObjects->FlushCommandQueue();
}

void EngineSystem::CommitCommandLists()
{
	m_cmdObjects->CommitCommandLists();
}
