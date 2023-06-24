#include "pch.h"
#include "Material.h"
#include "PolySurface.h"
#include "EngineSystem.h"
#include "GPUBufferUtils.h"

PolySurface::PolySurface()
{
	m_vertices = {};
	m_vertexCount = 0;
	m_vertexBufferCPU = nullptr;
	m_vertexBufferGPU = nullptr;
	m_vertexBufferUploader = nullptr;

	m_indices = {};
	m_indexCount = 0;
	m_indexBufferCPU = nullptr;
	m_indexBufferGPU = nullptr;
	m_indexBufferUploader = nullptr;

	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

PolySurface::~PolySurface()
{
}

void PolySurface::Commit()
{
	m_vertexCount = m_vertices.size();
	m_indexCount = m_indices.size();

	BuildVertexBuffer(EngineSystem::GetInst()->GetDevice().Get(), EngineSystem::GetInst()->GetCmdList().Get());
	BuildIndexBuffer(EngineSystem::GetInst()->GetDevice().Get(), EngineSystem::GetInst()->GetCmdList().Get());
}

void PolySurface::Update()
{
	EngineSystem::GetInst()->GetCmdList()->IASetVertexBuffers(0, 1, &GetVertexBufferView());
	EngineSystem::GetInst()->GetCmdList()->IASetIndexBuffer(&GetIndexBufferView());
	EngineSystem::GetInst()->GetCmdList()->IASetPrimitiveTopology(m_primitiveTopology);
}

void PolySurface::Render()
{
	EngineSystem::GetInst()->GetCmdList()->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
}

void PolySurface::RenderPolySurfaceShape(const string& name)
{
	int indexCount = m_polySurfaceShapes[name].IndexCount;
	int indexStart = m_polySurfaceShapes[name].StartIndexLocation;
	int vertexStart = m_polySurfaceShapes[name].StartVertexLocation;

	EngineSystem::GetInst()->GetCmdList()->DrawIndexedInstanced(indexCount, 1, indexStart, vertexStart, 0);
}

void PolySurface::SetVertices(vector<Vertex> vertices)
{
	m_vertices = vertices;
}

void PolySurface::SetIndices(vector<uint32> indices)
{
	m_indices = indices;
}

void PolySurface::SetPolySurfaceShapes(const string& name, PolySurfaceShape info)
{
	m_polySurfaceShapes[name] = info;
}

void PolySurface::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY primitiveTopology)
{
	m_primitiveTopology = primitiveTopology;
}

void PolySurface::SetMaterial(const string& materialName, const string& polySurfaceShapeInfoName)
{
	if (polySurfaceShapeInfoName == "")
	{
		for (auto polySurfaceShape : m_polySurfaceShapes)
		{
			m_polySurfaceShapes[polySurfaceShape.first].MaterialName = materialName;
		}
	}
	else
	{
		m_polySurfaceShapes[polySurfaceShapeInfoName].MaterialName = materialName;
	}
}

unordered_map<string, PolySurfaceShape> PolySurface::GetPolySurfaceShapes()
{
	return m_polySurfaceShapes;
}

void PolySurface::Create(string name, vector<Vertex> vertices, vector<uint32> indices)
{
	m_name = name;
	m_vertices = vertices;
	m_indices = indices;
}

void PolySurface::CreateBox(float width, float height, float depth,
	uint32 numSubdivisions, const string& mat)
{
#pragma region
	float w2 = 0.5f * width;
	float h2 = 0.5f * height;
	float d2 = 0.5f * depth;

	// Define vertices.
	vector<Vertex> vertices(24);
	// Front-Face
	vertices[0] = Vertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[1] = Vertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[2] = Vertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[3] = Vertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Back-Face
	vertices[4] = Vertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[5] = Vertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[6] = Vertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[7] = Vertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	vertices[8] = Vertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[9] = Vertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[10] = Vertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	vertices[11] = Vertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	vertices[12] = Vertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
	vertices[13] = Vertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices[14] = Vertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices[15] = Vertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	vertices[16] = Vertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	vertices[17] = Vertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	vertices[18] = Vertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	vertices[19] = Vertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	vertices[20] = Vertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	vertices[21] = Vertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	vertices[22] = Vertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
	vertices[23] = Vertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);

	//	Define indices.
	vector<uint32> indices(36);
	// Fill in the front face index data
	indices[0] = 0;	indices[1] = 1;	indices[2] = 2;
	indices[3] = 0;	indices[4] = 2;	indices[5] = 3;

	// Fill in the back face index data
	indices[6] = 4;	indices[7] = 5; indices[8] = 6;
	indices[9] = 4;	indices[10] = 6; indices[11] = 7;

	// Fill in the top face index data
	indices[12] = 8;	indices[13] = 9;	indices[14] = 10;
	indices[15] = 8;	indices[16] = 10;	indices[17] = 11;

	// Fill in the bottom face index data
	indices[18] = 12;	indices[19] = 13;	indices[20] = 14;
	indices[21] = 12;	indices[22] = 14;	indices[23] = 15;

	// Fill in the left face index data
	indices[24] = 16; indices[25] = 17; indices[26] = 18;
	indices[27] = 16; indices[28] = 18; indices[29] = 19;

	// Fill in the right face index data
	indices[30] = 20; indices[31] = 21; indices[32] = 22;
	indices[33] = 20; indices[34] = 22; indices[35] = 23;

	numSubdivisions = min<uint32>(numSubdivisions, 6u);

	//for (uint32 i = 0; i < numSubdivisions; ++i)
	//	Subdivide(vertices, indices);

	CreatePolySurface("Box", vertices, indices, mat);
#pragma endregion
}

void PolySurface::CreateGrid(float width, float depth, uint32 m, uint32 n, const string& mat)
{
#pragma region
	uint32 vertexCount = m * n;
	uint32 faceCount = (m - 1) * (n - 1) * 2;

	float halfWidth = 0.5f * width;
	float halfDepth = 0.5f * depth;

	float dx = width / (n - 1);
	float dz = depth / (m - 1);

	float du = 1.0f / (n - 1);
	float dv = 1.0f / (m - 1);

	vector<Vertex> vertices;
	vertices.resize(vertexCount);
	for (uint32 i = 0; i < m; ++i)
	{
		float z = halfDepth - i * dz;

		for (uint32 j = 0; j < n; ++j)
		{
			float x = -halfWidth + j * dx;

			vertices[i * n + j].Position = XMFLOAT3(x, 0.0f, z);
			vertices[i * n + j].Normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
			vertices[i * n + j].TangentU = XMFLOAT3(1.0f, 0.0f, 0.0f);

			vertices[i * n + j].UV.x = j * du;
			vertices[i * n + j].UV.y = i * dv;
		}
	}

	vector<uint32> indices;
	indices.resize(faceCount * 3);

	uint32 k = 0;
	for (uint32 i = 0; i < m - 1; ++i)
	{
		for (uint32 j = 0; j < n - 1; ++j)
		{
			indices[k] = i * n + j;
			indices[k + 1] = i * n + j + 1;
			indices[k + 2] = (i + 1) * n + j;

			indices[k + 3] = (i + 1) * n + j;
			indices[k + 4] = i * n + j + 1;
			indices[k + 5] = (i + 1) * n + j + 1;

			k += 6;
		}
	}

	CreatePolySurface("Grid", vertices, indices, mat);

#pragma endregion
}

void PolySurface::CreateSphere(float radius, uint32 sliceCount, uint32 stackCount, const string& mat)
{
#pragma region
	vector<Vertex> vertices;

	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	vertices.push_back(topVertex);

	float stepPhi = XM_PI / stackCount;
	float stepTheta = 2.0f * XM_PI / sliceCount;

	for (uint32 i = 1; i <= stackCount; ++i)
	{
		float phi = i * stepPhi;

		for (uint32 j = 0; j <= sliceCount; ++j)
		{
			float theta = j * stepTheta;

			Vertex v;

			//	spherical to cartesian.
			v.Position.x = radius * sinf(phi) * cosf(theta);
			v.Position.y = radius * cosf(phi);
			v.Position.z = radius * sinf(phi) * sinf(theta);

			//	Partial derivative of P with respect to theta.
			v.TangentU.x = -radius * sinf(phi) * cosf(theta);
			v.TangentU.y = 0.0f;
			v.TangentU.z = +radius * sinf(phi) * cosf(theta);

			XMVECTOR T = XMLoadFloat3(&v.TangentU);
			XMStoreFloat3(&v.TangentU, XMVector3Normalize(T));

			XMVECTOR P = XMLoadFloat3(&v.Position);
			XMStoreFloat3(&v.Normal, XMVector3Normalize(P));

			v.UV.x = theta / XM_2PI;
			v.UV.y = phi / XM_PI;

			vertices.push_back(v);
		}
	}

	Vertex botVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	vertices.push_back(botVertex);

	vector<uint32> indices;
	for (uint32 i = 1; i <= sliceCount; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint32 baseIndex = 1;
	uint32 ringVertexCount = sliceCount + 1;
	for (uint32 i = 0; i < sliceCount - 2; ++i)
	{
		for (uint32 j = 0; j < stackCount; ++j)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	uint32 southPoleIndex = (uint32)vertices.size() - 1;

	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32 i = 0; i < sliceCount; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	CreatePolySurface("Sphere", vertices, indices, mat);
#pragma endregion
}

void PolySurface::CreatePolySurface(const string& name, vector<Vertex> vertices, vector<uint32> indices, const string& mat)
{
#pragma region
	PolySurfaceShape polySurfaceShape;
	polySurfaceShape.IndexCount = indices.size();
	polySurfaceShape.StartIndexLocation = m_indices.size();
	polySurfaceShape.StartVertexLocation = m_vertices.size();
	polySurfaceShape.MaterialName = mat;

	SetPolySurfaceShapes(name, polySurfaceShape);

	for (int i = 0; i < vertices.size(); i++)
	{
		m_vertices.push_back(vertices[i]);
	}

	for (int i = 0; i < indices.size(); i++)
	{
		m_indices.push_back(indices[i]);
	}

	m_vertexCount = m_vertices.size();
	m_indexCount = m_indices.size();
#pragma endregion
}

Vertex PolySurface::MidPoint(const Vertex& v0, const Vertex& v1)
{
#pragma region
	XMVECTOR p0 = XMLoadFloat3(&v0.Position);
	XMVECTOR p1 = XMLoadFloat3(&v1.Position);

	XMVECTOR n0 = XMLoadFloat3(&v0.Normal);
	XMVECTOR n1 = XMLoadFloat3(&v1.Normal);

	XMVECTOR pos = 0.5f * (p0 + p1);
	XMVECTOR normal = 0.5 * (n0 + n1);

	Vertex vtx;
	XMStoreFloat3(&vtx.Position, pos);
	XMStoreFloat3(&vtx.Normal, normal);
#pragma endregion
	return vtx;
}

void PolySurface::Subdivide(vector<Vertex>& vertices, vector<uint32>& indices)
{
#pragma region
	vector<Vertex> inputVertices = vertices;
	vector<uint32> inputIndices = indices;

	vertices.resize(0);
	indices.resize(0);

	uint32 numTris = (uint32)indices.size() / 3;
	for (uint32 i = 0; i < numTris; ++i)
	{
		Vertex v0 = inputVertices[inputIndices[i * 3 + 0]];
		Vertex v1 = inputVertices[inputIndices[i * 3 + 1]];
		Vertex v2 = inputVertices[inputIndices[i * 3 + 2]];

		//  Generate the midpoints.
		Vertex m0 = MidPoint(v0, v1);
		Vertex m1 = MidPoint(v1, v2);
		Vertex m2 = MidPoint(v0, v2);

		//  Add New Geometry.
		vertices.push_back(v0);
		vertices.push_back(v1);
		vertices.push_back(v2);
		vertices.push_back(m0);
		vertices.push_back(m1);
		vertices.push_back(m2);

		indices.push_back(i * 6 + 0);
		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 5);

		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 4);
		indices.push_back(i * 6 + 5);

		indices.push_back(i * 6 + 5);
		indices.push_back(i * 6 + 4);
		indices.push_back(i * 6 + 2);

		indices.push_back(i * 6 + 3);
		indices.push_back(i * 6 + 1);
		indices.push_back(i * 6 + 4);
	}
#pragma endregion
}

D3D12_VERTEX_BUFFER_VIEW PolySurface::GetVertexBufferView()
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = m_vertexBufferGPU->GetGPUVirtualAddress();
	vbv.SizeInBytes = m_vertexCount * sizeof(Vertex);
	vbv.StrideInBytes = sizeof(Vertex);

	return vbv;
}
void PolySurface::BuildVertexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList)
{
	const UINT vbByteSize = m_vertexCount * sizeof(Vertex);
	D3DCreateBlob(vbByteSize, &m_vertexBufferCPU);
	CopyMemory(m_vertexBufferCPU->GetBufferPointer(), m_vertices.data(), vbByteSize);

	m_vertexBufferGPU = CreateDefaultBuffer(pDevice, pCmdList, m_vertices.data(), vbByteSize, m_vertexBufferUploader);
}

D3D12_INDEX_BUFFER_VIEW PolySurface::GetIndexBufferView()
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = m_indexBufferGPU->GetGPUVirtualAddress();
	ibv.Format = INDEX_BUFFER_FORMAT;
	ibv.SizeInBytes = m_indexCount * sizeof(uint32);

	return ibv;
}

void PolySurface::BuildIndexBuffer(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList)
{
	const UINT ibByteSize = m_indexCount * sizeof(uint32);
	D3DCreateBlob(ibByteSize, &m_indexBufferCPU);
	CopyMemory(m_indexBufferCPU->GetBufferPointer(), m_indices.data(), ibByteSize);

	m_indexBufferGPU = CreateDefaultBuffer(pDevice, pCmdList, m_indices.data(), ibByteSize, m_indexBufferUploader);
}
