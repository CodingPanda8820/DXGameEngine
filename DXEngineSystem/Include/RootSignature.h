#pragma once

enum STATIC_SAMPLER_TYPE : uint8
{
	POINT_WRAP,
	POINT_CLAMP,
	LINEAR_WRAP,
	LINEAR_CLAMP,
	ANISOTROPIC_WRAP,
	ANISOTROPIC_CLAMP,
	SHADOW,
	COUNT,
};

enum
{
	STATIC_SAMPLER_COUNT = static_cast<uint8>(STATIC_SAMPLER_TYPE::COUNT),
};

class RootSignature
{
public:
	RootSignature();
	~RootSignature();

public:
	bool Init(ComPtr<ID3D12Device> device);

public:
	ComPtr<ID3D12RootSignature> GetRootSignature();

	void SetGraphicsRootSignature(ComPtr<ID3D12GraphicsCommandList> cmdList);

private:
	void BuildRootSignature(ComPtr<ID3D12Device> device);
	array<const CD3DX12_STATIC_SAMPLER_DESC, STATIC_SAMPLER_COUNT> StaticSamplers();

private:
	ComPtr<ID3D12RootSignature> m_rootSignature;
};

