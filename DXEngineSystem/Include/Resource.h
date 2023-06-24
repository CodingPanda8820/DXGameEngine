#pragma once
#include "CustomBuffer.h"
#include "EngineUtils.h"

class Resource
{
public:
	Resource();
	~Resource();

private:
	UINT64 m_fenceValue = 0;

	ComPtr<ID3D12CommandAllocator> m_cmdAllocator;

	unique_ptr<UploadBuffer<CBObject>>	m_cbObject;
	unique_ptr<UploadBuffer<CBPass>>	m_cbPass;
};

