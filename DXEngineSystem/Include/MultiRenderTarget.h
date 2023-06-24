#pragma once
#include "Texture.h"

enum class RENDER_TARGET_TYPE : uint8
{
	SWAP_CHAIN,
	GBUFFER,
	LIGHTING,
	SHADOW,
	COUNT,
};

enum class WAIT_SYNC_TYPE : uint8
{
	RENDER_TARGET_TO_RESOURCE,
	RESOURCE_TO_RENDER_TARGET,
	COUNT,
};

enum
{
	RENDER_TARGET_TYPE_COUNT = static_cast<uint8>(RENDER_TARGET_TYPE::COUNT),
	RENDER_TARGET_SHADOW_COUNT = 1,
	RENDER_TARGET_GBUFFER_COUNT = 3,
	RENDER_TARGET_LIGHTING_COUNT = 2,
};

struct RenderTarget
{
	shared_ptr<Texture> Texture;
	XMFLOAT4			Clear;
};

class MultiRenderTarget
{
public:
	void Create(ComPtr<ID3D12Device> device, RENDER_TARGET_TYPE type,
		vector<RenderTarget>& renderTargets, shared_ptr<Texture> depthStencilTexture);

	void OMSetRenderTarget(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32 count, uint32 offset);
	void OMSetRenderTargets(ComPtr<ID3D12GraphicsCommandList> cmdList);

	void ClearRenderTargetView(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32 index);
	void ClearRenderTargetViews(ComPtr<ID3D12GraphicsCommandList> cmdList);

	shared_ptr<Texture> GetRenderTarget(uint32 index);
	shared_ptr<Texture> GetDepthStencil();

	void WaitSync(ComPtr<ID3D12GraphicsCommandList> cmdList, WAIT_SYNC_TYPE type);

private:
	RENDER_TARGET_TYPE				m_type;
	vector<RenderTarget>			m_rts;
	uint32							m_rtsCount;

	shared_ptr<Texture>				m_ds;

private:
	ComPtr<ID3D12DescriptorHeap>	m_rtvHeap;
	uint32							m_rtvHeapSize;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_rtvCpuHandle;

	ComPtr<ID3D12DescriptorHeap>	m_dsvHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_dsvCpuHandle;
};

