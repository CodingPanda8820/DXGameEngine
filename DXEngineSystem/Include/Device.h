#pragma once
#include "EngineUtils.h"

class Device
{
public:
	Device();
	~Device();

public:
	bool Init();

public:
	ComPtr<ID3D12Device> GetDevice() { return m_device; }
	ComPtr<IDXGIFactory> GetDXGI() { return m_dxgi; }

private:
	ComPtr<ID3D12Device>	m_device;
	ComPtr<IDXGIFactory>	m_dxgi;
};

