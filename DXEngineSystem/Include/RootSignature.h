#pragma once
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
	array<const CD3DX12_STATIC_SAMPLER_DESC, 6> StaticSamplers();

private:
	ComPtr<ID3D12RootSignature> m_rootSignature;
};

