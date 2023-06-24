#include "pch.h"
#include "RootSignature.h"
#include "EngineUtils.h"

RootSignature::RootSignature()
{
}

RootSignature::~RootSignature()
{
}

bool RootSignature::Init(ComPtr<ID3D12Device> device)
{
	BuildRootSignature(device);

	return true;
}

ComPtr<ID3D12RootSignature> RootSignature::GetRootSignature()
{
	return m_rootSignature;
}

void RootSignature::SetGraphicsRootSignature(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	cmdList->SetGraphicsRootSignature(m_rootSignature.Get());
}

void RootSignature::BuildRootSignature(ComPtr<ID3D12Device> device)
{
	CD3DX12_DESCRIPTOR_RANGE srvTexture2DTable;
	srvTexture2DTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, REGISTER_ID(TREGISTER_TYPE::COUNT), 0);

	CD3DX12_ROOT_PARAMETER rootParms[6];
	rootParms[0].InitAsConstantBufferView(0);	//	Geometry
	rootParms[1].InitAsConstantBufferView(1);	//	Material
	rootParms[2].InitAsConstantBufferView(2);	//	  Lights
	rootParms[3].InitAsConstantBufferView(3);	//	  Camera
	rootParms[4].InitAsConstantBufferView(4);	//	   Timer
	rootParms[5].InitAsDescriptorTable(1, &srvTexture2DTable, D3D12_SHADER_VISIBILITY_PIXEL);

	auto staticSamplers = StaticSamplers();

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(_countof(rootParms), rootParms, (UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> errorBlob = nullptr;
	ComPtr<ID3DBlob> serializeRootSignature = nullptr;

	HRESULT hr = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializeRootSignature.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	device->CreateRootSignature(0, serializeRootSignature->GetBufferPointer(),
		serializeRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
}

array<const CD3DX12_STATIC_SAMPLER_DESC, STATIC_SAMPLER_COUNT> RootSignature::StaticSamplers()
{
	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1,
		D3D12_FILTER_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP
	);

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3,
		D3D12_FILTER_MIN_MAG_MIP_LINEAR,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP
	);

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,
		0.0f, 8
	);

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5,
		D3D12_FILTER_ANISOTROPIC,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,
		0.0f, 8
	);

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		6,
		D3D12_FILTER_COMPARISON_MIN_MAG_MIP_POINT,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,
		0.0f,
		16,
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK
	);

	return { pointWrap, pointClamp, linearWrap, linearClamp, anisotropicWrap, anisotropicClamp, shadow };
}
