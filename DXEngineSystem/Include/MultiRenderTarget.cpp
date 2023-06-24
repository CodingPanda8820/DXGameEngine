#include "pch.h"
#include "MultiRenderTarget.h"

void MultiRenderTarget::Create(ComPtr<ID3D12Device> device, RENDER_TARGET_TYPE type,
								vector<RenderTarget>& renderTargets, shared_ptr<Texture> depthStencilTexture)
{
	m_type = type;
	m_rts = renderTargets;
	m_rtsCount = static_cast<uint32>(renderTargets.size());
	m_ds = depthStencilTexture;

	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeap = {};
	descriptorHeap.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	descriptorHeap.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	descriptorHeap.NodeMask = 0;
	descriptorHeap.NumDescriptors = m_rtsCount;

	device->CreateDescriptorHeap(&descriptorHeap, IID_PPV_ARGS(&m_rtvHeap));

	m_rtvHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_rtvCpuHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
	
	for (uint32 i = 0; i < m_rtsCount; i++)
	{
		uint32 destSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE destCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvCpuHandle, i * m_rtvHeapSize);

		uint32 srcSize = 1;
		D3D12_CPU_DESCRIPTOR_HANDLE srcCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rts[i].Texture->GetRTVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());

		device->CopyDescriptors(1, &destCpuHandle, &destSize, 1, &srcCpuHandle, &srcSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}
	
	m_dsvCpuHandle = m_ds->GetDSVDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
}

void MultiRenderTarget::OMSetRenderTarget(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32 count, uint32 offset)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvCpuHandle, offset * m_rtvHeapSize);
	cmdList->OMSetRenderTargets(count, &rtvCpuHandle, FALSE, &m_dsvCpuHandle);
}

void MultiRenderTarget::OMSetRenderTargets(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	cmdList->OMSetRenderTargets(m_rtsCount, &m_rtvCpuHandle, TRUE, &m_dsvCpuHandle);
}

void MultiRenderTarget::ClearRenderTargetView(ComPtr<ID3D12GraphicsCommandList> cmdList, uint32 index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvCpuHandle, index * m_rtvHeapSize);
	cmdList->ClearRenderTargetView(rtvCpuHandle, &m_rts[index].Clear.x, 0, nullptr);
	cmdList->ClearDepthStencilView(m_dsvCpuHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void MultiRenderTarget::ClearRenderTargetViews(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	WaitSync(cmdList, WAIT_SYNC_TYPE::RESOURCE_TO_RENDER_TARGET);

	for (uint32 i = 0; i < m_rtsCount; i++)
	{
		D3D12_CPU_DESCRIPTOR_HANDLE rtvCpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvCpuHandle, i * m_rtvHeapSize);
		cmdList->ClearRenderTargetView(rtvCpuHandle, &m_rts[i].Clear.x, 0, nullptr);
	}

	cmdList->ClearDepthStencilView(m_dsvCpuHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void MultiRenderTarget::WaitSync(ComPtr<ID3D12GraphicsCommandList> cmdList, WAIT_SYNC_TYPE type)
{
	D3D12_RESOURCE_BARRIER resourceBarriers[sizeof(m_rtsCount)];

	switch (type)
	{
	case WAIT_SYNC_TYPE::RENDER_TARGET_TO_RESOURCE:
	{
		for (int i = 0; i < m_rtsCount; ++i)
		{
			resourceBarriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_rts[i].Texture->GetResource().Get(),
				D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
		}
		break;
	}
	case WAIT_SYNC_TYPE::RESOURCE_TO_RENDER_TARGET:
	{
		for (int i = 0; i < m_rtsCount; ++i)
		{
			resourceBarriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(m_rts[i].Texture->GetResource().Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
		}
		break;
	}
	default:
		break;
	}

	cmdList->ResourceBarrier(m_rtsCount, resourceBarriers);
}

shared_ptr<Texture> MultiRenderTarget::GetRenderTarget(uint32 index)
{
	return m_rts[index].Texture;
}

shared_ptr<Texture> MultiRenderTarget::GetDepthStencil()
{
	return m_ds;
}
