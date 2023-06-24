#include "pch.h"
#include "UIShape.h"
#include "GPUBufferUtils.h"

UIShape::UIShape()
{
	m_width = 720;
	m_height = 480;

	m_vertices.resize(4);
	m_indices.resize(6);

	m_vertexCount = m_vertices.size();
	m_vertexBufferCPU = nullptr;
	m_vertexBufferGPU = nullptr;
	m_vertexBufferUploader = nullptr;

	m_indexCount = m_indices.size();
	m_indexBufferCPU = nullptr;
	m_indexBufferGPU = nullptr;
	m_indexBufferUploader = nullptr;
}

UIShape::~UIShape()
{
}

void UIShape::Update()
{
}

void UIShape::Render(ComPtr<ID3D12GraphicsCommandList> cmdList)
{
	cmdList->IASetVertexBuffers(0, 1, &GetVertexBufferView());
	cmdList->IASetIndexBuffer(&GetIndexBufferView());
	cmdList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	cmdList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}

void UIShape::Create(string name, float width, float height)
{
	m_name = name;

	m_vertices[0] = Vertex(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	m_vertices[1] = Vertex(width, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	m_vertices[2] = Vertex(0.0f, height, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	m_vertices[3] = Vertex(0.0f, height, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	m_indices[0] = 0;	m_indices[1] = 1;	m_indices[2] = 2;
	m_indices[3] = 0;	m_indices[4] = 2;	m_indices[5] = 3;
}

void UIShape::Commit(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList)
{
	BuildVertexBuffer(pDevice, pCmdList);
	BuildIndexBuffer(pDevice, pCmdList);
}

D3D12_VERTEX_BUFFER_VIEW UIShape::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = m_vertexBufferGPU->GetGPUVirtualAddress();
	vbv.SizeInBytes = m_vertexCount * sizeof(Vertex);
	vbv.StrideInBytes = sizeof(Vertex);

	return vbv;
}
D3D12_INDEX_BUFFER_VIEW UIShape::GetIndexBufferView()
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = m_indexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = INDEX_BUFFER_FORMAT;
	ibv.SizeInBytes = m_indexCount * sizeof(uint32);

	return ibv;
}
void UIShape::BuildVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList)
{
	const UINT vbByteSize = m_vertexCount * sizeof(Vertex);
	D3DCreateBlob(vbByteSize, &m_vertexBufferCPU);
	CopyMemory(m_vertexBufferCPU->GetBufferPointer(), m_vertices.data(), vbByteSize);

	m_vertexBufferGPU = CreateDefaultBuffer(pDevice, pCmdList, m_vertices.data(), vbByteSize, m_vertexBufferUploader);
}
void UIShape::BuildIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList)
{
	const UINT ibByteSize = m_indexCount * sizeof(uint32);
	D3DCreateBlob(ibByteSize, &m_indexBufferCPU);
	CopyMemory(m_indexBufferCPU->GetBufferPointer(), m_indices.data(), ibByteSize);

	m_indexBufferGPU = CreateDefaultBuffer(pDevice, pCmdList, m_indices.data(), ibByteSize, m_indexBufferUploader);
}
