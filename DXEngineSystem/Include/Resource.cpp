#include "pch.h"
#include "Resource.h"

Resource::Resource()
{
	//device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
	//	IID_PPV_ARGS(m_cmdAllocator.GetAddressOf()));

	//m_cbObject = make_unique<UploadBuffer<CBObject>>(device, passCount, objectCount);
	//m_cbPass = make_unique<UploadBuffer<CBPass>>(device, passCount, objectCount);
}

Resource::~Resource()
{
}
