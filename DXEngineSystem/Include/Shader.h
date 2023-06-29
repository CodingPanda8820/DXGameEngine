#pragma once

#pragma region TYPE
enum class SHADER_TYPE : uint8
{
	FORWARD,
	DEFERRED,
	LIGHTING,
	SHADOW,
	COUNT,
};

enum class SHADER_CODE_TYPE : uint8
{
	VS,
	HS,
	DS,
	GS,
	CS,
	PS,
	COUNT,
};

enum class RASTERIZER_TYPE : uint8
{
	CULL_BACK,
	CULL_FRONT,
	CULL_NONE,
	WIREFRAME,
	CULL_BACK_DEPTH_BIAS,
	COUNT,
};

enum class DEPTH_STENCIL_TYPE : uint8
{
	LESS,
	LESS_EQUAL,
	GREATER,
	GREATER_EQUAL,
	DEPTH_UNABLE_WRITE,
	DEPTH_UNABLE_NOT_WRITE,
	DEPTH_LESS_NOT_WRITE,
	COUNT,
};

enum class BLEND_TYPE : uint8
{
	DEFAULT,
	ALPHA_BLENDING,
	ADD,
	COUNT,
};

struct ShaderInfo
{
	SHADER_TYPE						Type = SHADER_TYPE::FORWARD;
	RASTERIZER_TYPE					Rasterizer = RASTERIZER_TYPE::CULL_BACK;
	DEPTH_STENCIL_TYPE				DepthStencil = DEPTH_STENCIL_TYPE::LESS;
	BLEND_TYPE						Blend = BLEND_TYPE::DEFAULT;
	D3D12_PRIMITIVE_TOPOLOGY_TYPE	Topology = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
};
#pragma endregion

class Shader
{
public:
	Shader();
	~Shader();

public:
	bool Create(const string& name, ShaderInfo info = ShaderInfo());
	void Render();

public:
	ComPtr<ID3DBlob> CompileShader(const wstring& shaderFilePath, const D3D_SHADER_MACRO* defines, const string& entryPoint, const string& version);
	void AppendInputElement(LPCSTR name, UINT index, DXGI_FORMAT format, UINT slot, UINT byteOffset, UINT dataType, UINT stepRate);

public:
	void CreatePSO(SHADER_TYPE type);
	void SetBlendType(BLEND_TYPE type);
	void SetRasterizerType(RASTERIZER_TYPE type);
	void SetDepthStencilType(DEPTH_STENCIL_TYPE type);
	void SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE type);
	void LoadShaderByteCode(const wstring& shaderFilePath, const string& name, SHADER_CODE_TYPE shaderType);

public:
	const ComPtr<ID3D12PipelineState> GetPSO(const string& name) { return m_pso; }

private:
	string m_name;

	ComPtr<ID3DBlob> m_vsByteCode;
	ComPtr<ID3DBlob> m_hsByteCode;
	ComPtr<ID3DBlob> m_dsByteCode;
	ComPtr<ID3DBlob> m_gsByteCode;
	ComPtr<ID3DBlob> m_csByteCode;
	ComPtr<ID3DBlob> m_psByteCode;

	vector<D3D12_INPUT_ELEMENT_DESC> m_inputElements;

	D3D12_BLEND_DESC				 m_blendDesc;
	D3D12_RASTERIZER_DESC			 m_rasterizerDesc;
	D3D12_DEPTH_STENCIL_DESC		 m_depthStencilDesc;

	D3D12_PRIMITIVE_TOPOLOGY_TYPE	 m_primitiveType;

	ComPtr<ID3D12PipelineState>		 m_pso;
};

