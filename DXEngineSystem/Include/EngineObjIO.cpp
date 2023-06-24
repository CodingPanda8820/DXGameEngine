#include "pch.h"
#include "EngineObjIO.h"

void EngineObjIO::LoadOBJFile(const wstring& filePath, vector<Vertex>& vertices, vector<uint32>& indices)
{
	OBJData* objData = new OBJData;
	CompileOBJFile(filePath, objData);

	objData->vertices.resize(objData->positions.size());

	Vertex vertex;
	for (OBJFace face : objData->faces)
	{
		vector<uint32> indices;

		//	define vertices;
		for (OBJVertex vtx : face.vertices)
		{
			int idPosition = vtx.idPosition - 1;
			int idUV = vtx.idUV - 1;
			int idNormal = vtx.idNormal - 1;

			vertex.Position = objData->positions[idPosition];
			vertex.UV = objData->uvs[idUV];
			vertex.Normal = objData->normals[idNormal];

			objData->vertices[idPosition] = vertex;

			indices.push_back(idPosition);
		}

		//	define indices;
		objData->indices.push_back(indices[0]);
		objData->indices.push_back(indices[1]);
		objData->indices.push_back(indices[2]);

		if (indices.size() > 3)
		{
			objData->indices.push_back(indices[0]);
			objData->indices.push_back(indices[2]);
			objData->indices.push_back(indices[3]);
		}
	}

	vertices = objData->vertices;
	indices = objData->indices;

	SAFE_DELETE(objData);
}

void EngineObjIO::CompileOBJFile(const wstring& filePath, OBJData* objData)
{
	ifstream objFile(filePath);

	if (!objFile.is_open())
		throw "ERROR::Can't open .obj!";

	XMFLOAT3	position;
	XMFLOAT2	uv;
	XMFLOAT3	normal;

	OBJFace		face;
	OBJVertex	vtx;

	string line = "";
	string prefix = "";

	stringstream strStream;
	while (true)
	{
		if (!getline(objFile, line))
			break;

		strStream.str(line);
		strStream >> prefix;

		if (prefix == "v")	//	position
		{
			strStream >> position.x >> position.y >> position.z;
			objData->positions.push_back(position);
		}
		else if (prefix == "vt")	// UV
		{
			strStream >> uv.x >> uv.y;
			objData->uvs.push_back(uv);
		}
		else if (prefix == "vn")	// normal
		{
			strStream >> normal.x >> normal.y >> normal.z;
			objData->normals.push_back(normal);
		}
		else if (prefix == "f")
		{
			while (true)
			{
				if (!strStream)
					break;

				strStream >> vtx.idPosition;
				strStream.ignore(1, '/');

				strStream >> vtx.idUV;
				strStream.ignore(1, '/');

				strStream >> vtx.idNormal;
				strStream.ignore(1, '/');

				face.vertices.push_back(vtx);

				if (strStream.peek() == ' ')
					strStream.ignore(1, ' ');
			}

			objData->faces.push_back(face);
			face.vertices = {};
		}

		strStream.clear();
	}
}
