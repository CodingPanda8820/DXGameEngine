#include "pch.h"
#include "Device.h"

Device::Device()
{
}

Device::~Device()
{

}

bool Device::Init()
{
#if defined(DEBUG) || defined(_DEBUG)
{
    ComPtr<ID3D12Debug> debuger;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debuger));
    debuger->EnableDebugLayer();
}
#endif

    //  Create IDXGIFactory & ID3D12Device
    CreateDXGIFactory(IID_PPV_ARGS(&m_dxgi));

    HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));

    //  Software Display Adapter
    //  WARP(Windows Advanced Rasterization Platform : Windows °í±Þ ·¡½ºÅÍÈ­ ÇÃ·§Æû)
    /*if (FAILED(hardwareResult))
    {
        ComPtr<IDXGIFactory> softwareWarpAdapter;
        m_dxgi->EnumWarpAdapter(IID_PPV_ARGS(&softwareWarpAdapter));
        D3D12CreateDevice(softwareWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
    }*/

    return true;
}
