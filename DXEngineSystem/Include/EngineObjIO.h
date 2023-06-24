#pragma once
#include "EngineUtils.h"

struct OBJVertex
{
	uint32 idPosition;
	uint32 idUV;
	uint32 idNormal;
};

struct OBJFace
{
	vector<OBJVertex> vertices;
};

struct OBJData
{
	vector<XMFLOAT3> positions;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT2> uvs;
	vector<OBJFace> faces;

	vector<Vertex> vertices;
	vector<uint32> indices;
};

class EngineObjIO
{
public:
	static void LoadOBJFile(const wstring& filePath, vector<Vertex>& vertices, vector<uint32>& indices);
	static void CompileOBJFile(const wstring& filePath, OBJData* objData);
};

