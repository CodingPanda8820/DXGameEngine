#pragma once
#include "EngineUtils.h"

class PolySurface
{
public:
	PolySurface();
	~PolySurface();

public:
	void Commit();
	void Update();
	void Render();
	void RenderPolySurfaceShape(const string& name);

public:
	void SetVertices(vector<Vertex> vertices);
	void SetIndices(vector<uint32> indices);
	void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology);
	void SetMaterial(const string& materialName, const string& polySurfaceShapeInfoName = "");
	void SetPolySurfaceShapes(const string& name, PolySurfaceShape info);

	D3D12_INDEX_BUFFER_VIEW						GetIndexBufferView();
	D3D12_VERTEX_BUFFER_VIEW					GetVertexBufferView();
	unordered_map<string, PolySurfaceShape>		GetPolySurfaceShapes();

public:
	void Create(string name, vector<Vertex> vertices = {}, vector<uint32> indices = {});
	void CreateBox(float width, float height, float depth, uint32 numSubdivisions, const string& mat = "");
	void CreateGrid(float width, float depth, uint32 m, uint32 n, const string& mat = "");
	void CreateSphere(float radius, uint32 sliceCount, uint32 stackCount, const string& mat = "");
	void CreatePolySurface(const string& name, vector<Vertex> vertices, vector<uint32> indices, const string& mat = "");

public:
	Vertex	MidPoint(const Vertex& v0, const Vertex& v1);
	void	Subdivide(vector<Vertex>& vertices, vector<uint32>& indices);

private:
	void BuildVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);
	void BuildIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);

private:
	string	m_name;

	vector<Vertex> m_vertices;
	vector<uint32> m_indices;

	D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology;

	unordered_map<string, PolySurfaceShape> m_polySurfaceShapes;

private:
	UINT					m_vertexCount;
	ComPtr<ID3DBlob>		m_vertexBufferCPU;
	ComPtr<ID3D12Resource>	m_vertexBufferGPU;
	ComPtr<ID3D12Resource>	m_vertexBufferUploader;

	UINT					m_indexCount;
	ComPtr<ID3DBlob>		m_indexBufferCPU;
	ComPtr<ID3D12Resource>	m_indexBufferGPU;
	ComPtr<ID3D12Resource>	m_indexBufferUploader;
};

