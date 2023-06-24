#pragma once
#include "EngineUtils.h"

class UIShape
{
public:
	UIShape();
	~UIShape();

public:
	void Update();
	void Render(ComPtr<ID3D12GraphicsCommandList> cmdList);

public:
	void Create(string name, float width, float height);
	void Commit(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);

public:
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

private:
	void BuildVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);
	void BuildIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);

private:
	string					m_name;

	uint8					m_width;
	uint8					m_height;

	vector<Vertex>			m_vertices;
	vector<uint32>			m_indices;

	UINT					m_vertexCount;
	ComPtr<ID3DBlob>		m_vertexBufferCPU;
	ComPtr<ID3D12Resource>	m_vertexBufferGPU;
	ComPtr<ID3D12Resource>	m_vertexBufferUploader;

	UINT					m_indexCount;
	ComPtr<ID3DBlob>		m_indexBufferCPU;
	ComPtr<ID3D12Resource>	m_indexBufferGPU;
	ComPtr<ID3D12Resource>	m_indexBufferUploader;
};

