#include "pch.h"
#include "Shader.h"
#include "EngineSystem.h"

Shader::Shader()
{
	m_blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	m_rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	m_depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	m_primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
}

Shader::~Shader()
{
}

bool Shader::Create(const string& name, ShaderInfo info)
{
	m_name = name;

	SetBlendType(info.Blend);
	SetRasterizerType(info.Rasterizer);
	SetDepthStencilType(info.DepthStencil);
	SetPrimitiveTopologyType(info.Topology);

	CreatePSO(info.Type);

	return true;
}

void Shader::Render()
{
	EngineSystem::GetInst()->GetCmdList()->SetPipelineState(m_pso.Get());
}

void Shader::LoadShaderByteCode(const wstring& shaderFilePath, const string& name, SHADER_CODE_TYPE shaderType)
{
	switch (shaderType)
	{
	case SHADER_CODE_TYPE::VS:
	{
		m_vsByteCode = CompileShader(shaderFilePath, nullptr, name, "vs_5_1");
		break;
	}
	case SHADER_CODE_TYPE::HS:
	{
		m_hsByteCode = CompileShader(shaderFilePath, nullptr, name, "hs_5_1");
		break;
	}
	case SHADER_CODE_TYPE::DS:
	{
		m_dsByteCode = CompileShader(shaderFilePath, nullptr, name, "ds_5_1");
		break;
	}
	case SHADER_CODE_TYPE::GS:
	{
		m_gsByteCode = CompileShader(shaderFilePath, nullptr, name, "gs_5_1");
		break;
	}
	case SHADER_CODE_TYPE::CS:
	{
		m_csByteCode = CompileShader(shaderFilePath, nullptr, name, "cs_5_1");
		break;
	}
	case SHADER_CODE_TYPE::PS:
	{
		m_psByteCode = CompileShader(shaderFilePath, nullptr, name, "ps_5_1");
		break;
	}
	}
}

void Shader::CreatePSO(SHADER_TYPE type)
{
	//D3D12_COMPUTE_PIPELINE_STATE_DESC
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	psoDesc.InputLayout = { m_inputElements.data(), (UINT)m_inputElements.size() };
	psoDesc.pRootSignature = EngineSystem::GetInst()->GetRootSignature().Get();
	psoDesc.RasterizerState = m_rasterizerDesc;
	psoDesc.BlendState = m_blendDesc;
	//psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	//psoDesc.BlendState.AlphaToCoverageEnable = true;
	psoDesc.DepthStencilState = m_depthStencilDesc;
	psoDesc.DSVFormat = DEPTH_STENCIL_FORMAT;
	psoDesc.PrimitiveTopologyType = m_primitiveType;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleDesc.Quality = 0;

#pragma region ShaderType
	switch (type)
	{
	case SHADER_TYPE::DEFERRED:
		psoDesc.NumRenderTargets = RENDER_TARGET_GBUFFER_COUNT;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psoDesc.RTVFormats[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psoDesc.RTVFormats[2] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		psoDesc.RTVFormats[3] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		break;
	case SHADER_TYPE::FORWARD:
		psoDesc.NumRenderTargets = 1;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_TYPE::LIGHTING:
		psoDesc.NumRenderTargets = RENDER_TARGET_LIGHTING_COUNT;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		psoDesc.RTVFormats[1] = DXGI_FORMAT_R8G8B8A8_UNORM;
		break;
	case SHADER_TYPE::SHADOW:
		psoDesc.NumRenderTargets = RENDER_TARGET_SHADOW_COUNT;
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R32_FLOAT;
		break;
	default:
		break;
	}

#pragma endregion

#pragma region ByteCode
	if (m_vsByteCode)
		psoDesc.VS = { reinterpret_cast<BYTE*>(m_vsByteCode->GetBufferPointer()), m_vsByteCode->GetBufferSize() };
	if (m_hsByteCode)
		psoDesc.HS = { reinterpret_cast<BYTE*>(m_hsByteCode->GetBufferPointer()), m_hsByteCode->GetBufferSize() };
	if (m_dsByteCode)
		psoDesc.DS = { reinterpret_cast<BYTE*>(m_dsByteCode->GetBufferPointer()), m_dsByteCode->GetBufferSize() };
	if (m_gsByteCode)
		psoDesc.GS = { reinterpret_cast<BYTE*>(m_gsByteCode->GetBufferPointer()), m_gsByteCode->GetBufferSize() };
	if (m_psByteCode)
		psoDesc.PS = { reinterpret_cast<BYTE*>(m_psByteCode->GetBufferPointer()), m_psByteCode->GetBufferSize() };
#pragma endregion

	EngineSystem::GetInst()->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
}

void Shader::SetBlendType(BLEND_TYPE type)
{

	D3D12_RENDER_TARGET_BLEND_DESC& renderTargetBlendDesc = m_blendDesc.RenderTarget[0];

	switch (type)
	{
	case BLEND_TYPE::DEFAULT:
		renderTargetBlendDesc.BlendEnable = FALSE;
		renderTargetBlendDesc.LogicOpEnable = FALSE;
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
		break;
	case BLEND_TYPE::ALPHA_BLENDING:
		renderTargetBlendDesc.BlendEnable = TRUE;
		renderTargetBlendDesc.LogicOpEnable = FALSE;
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_ZERO;
		break;
	case BLEND_TYPE::ADD:
		renderTargetBlendDesc.BlendEnable = TRUE;
		renderTargetBlendDesc.LogicOpEnable = FALSE;
		renderTargetBlendDesc.SrcBlend = D3D12_BLEND_ONE;
		renderTargetBlendDesc.DestBlend = D3D12_BLEND_ONE;
		break;
	default:
		break;
	}
}

void Shader::SetRasterizerType(RASTERIZER_TYPE type)
{

	switch (type)
	{
	case RASTERIZER_TYPE::CULL_BACK:
		m_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		m_rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		break;
	case RASTERIZER_TYPE::CULL_FRONT:
		m_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		m_rasterizerDesc.CullMode = D3D12_CULL_MODE_FRONT;
		break;
	case RASTERIZER_TYPE::CULL_NONE:
		m_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		m_rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case RASTERIZER_TYPE::WIREFRAME:
		m_rasterizerDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
		m_rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
		break;
	case  RASTERIZER_TYPE::CULL_BACK_DEPTH_BIAS:
		m_rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
		m_rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
		break;
	default:
		break;
	}
}

void Shader::SetDepthStencilType(DEPTH_STENCIL_TYPE type)
{
	switch (type)
	{
	case DEPTH_STENCIL_TYPE::LESS:
		m_depthStencilDesc.DepthEnable = TRUE;
		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::LESS_EQUAL:
		m_depthStencilDesc.DepthEnable = TRUE;
		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::GREATER:
		m_depthStencilDesc.DepthEnable = TRUE;
		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
		break;
	case DEPTH_STENCIL_TYPE::GREATER_EQUAL:
		m_depthStencilDesc.DepthEnable = TRUE;
		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		break;
	case DEPTH_STENCIL_TYPE::DEPTH_UNABLE_WRITE:
		m_depthStencilDesc.DepthEnable = FALSE;
		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		break;
	case DEPTH_STENCIL_TYPE::DEPTH_UNABLE_NOT_WRITE:
		m_depthStencilDesc.DepthEnable = FALSE;
		m_depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	case DEPTH_STENCIL_TYPE::DEPTH_LESS_NOT_WRITE:
		m_depthStencilDesc.DepthEnable = TRUE;
		m_depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		m_depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		break;
	}
}

void Shader::SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type)
{
	m_primitiveType = type;
}

ComPtr<ID3DBlob> Shader::CompileShader(const wstring& shaderFilePath, const D3D_SHADER_MACRO* defines, const string& entryPoint, const string& version)
{
	UINT compileFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_ENABLE_UNBOUNDED_DESCRIPTOR_TABLES;
#endif

	HRESULT hr = S_OK;

	ComPtr<ID3DBlob> byteCode = nullptr;
	ComPtr<ID3DBlob> errors;

	hr = D3DCompileFromFile(shaderFilePath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint.c_str(), version.c_str(), compileFlags, 0, &byteCode, &errors);

	if (errors != nullptr)
		OutputDebugStringA((char*)errors->GetBufferPointer());

	return byteCode;
}

void Shader::AppendInputElement(LPCSTR name, UINT index, DXGI_FORMAT format, UINT slot,
	UINT byteOffset, UINT dataType, UINT stepRate)
{
	D3D12_INPUT_ELEMENT_DESC inputElement;
	inputElement.SemanticName = name;
	inputElement.SemanticIndex = index;
	inputElement.Format = format;
	inputElement.InputSlot = slot;
	inputElement.AlignedByteOffset = byteOffset;
	switch ((D3D12_INPUT_CLASSIFICATION)dataType)
	{
	case D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA:
	{
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
		break;
	}
	case D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA:
	{
		inputElement.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		break;
	}
	default:
		break;
	}
	inputElement.InstanceDataStepRate = stepRate;

	m_inputElements.push_back(inputElement);
}

