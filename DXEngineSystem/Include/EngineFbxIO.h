#pragma once
#include "EngineUtils.h"
//	Comment
//	Mesh = Set of Polygon

struct FbxSkeletonInfo
{
	int32 depth;
	double Weight;
};

struct SkinWeight
{

};

struct FbxSurfaceMaterialInfo
{
	FbxDouble3	Diffuse;
	FbxDouble	DiffuseFactor;
	FbxDouble3	Specular;
	FbxDouble	SpecularFactor;
	FbxDouble3	Ambient;
	FbxDouble	AmbientFactor;
	FbxDouble3	Shininess;
	FbxDouble	Pad1;

	string		Name;

	unordered_map<string, wstring> TexturePaths;
};

struct FbxMeshShapeInfo
{
	string MaterialName = "";

	uint32 VertexCount = 0;
	uint32 StartVertexLocation = 0;

	uint32 IndexCount = 0;
	uint32 StartIndexLocation = 0;
};

struct FbxMeshInfo
{
	string Name;

	vector<Point>				Points;
	vector<uint32>				Indices;
	vector<Vertex>				Vertices;
	vector<FbxMeshShapeInfo>	MeshShapeInfo;

	//vector<SkinWeight>	SkinWeights;
	//bool	Animated;
};

struct FbxSceneInfo
{
	vector<string>	Meshes;
	vector<string>	Materials;
	vector<string>	TexturePaths;
};

class EngineFbxIO
{
public:
	EngineFbxIO();
	~EngineFbxIO();

public:
	void Load(const wstring& filePath);
	void Import(const wstring& filePath);

	void ParseNode(FbxNode* node);

	FbxMeshInfo LoadMesh(const string& nodeName, FbxMesh* fbxMesh, vector<FbxSurfaceMaterialInfo> localMaterials);
	FbxSurfaceMaterialInfo LoadMaterial(FbxSurfaceMaterial* fbxMaterial);

private:
	void GetVertexPoint(Point point, Vertex* vertex);
	void GetVertexNormal(FbxMesh* mesh, Vertex* vertex, int32 index, int32 pointIndex);
	void GetVertexTangent(FbxMesh* mesh, Vertex* vertex, int32 index, int32 pointIndex);
	void GetVertexUV(FbxMesh* mesh, Vertex* vertex, int32 index, int32 uvIndex);

	template<typename T>
	void GetMaterialProperty(T& target, FbxSurfaceMaterial* material, const char* name);
	void GetMaterialTexturePath(wstring& filePath, FbxSurfaceMaterial* material, const char* name);

private:
	FbxManager*				m_fbxManager;
	FbxImporter*			m_fbxImporter;
	FbxScene*				m_fbxScene;

public:
	FbxSceneInfo GetFBXSceneInfo();

	unordered_map<string, FbxMeshInfo>					GetFBXMeshes();
	unordered_map<string, FbxSurfaceMaterialInfo>		GetFBXMaterials();

	unordered_map<string, PolySurfaceInfo>				GetPolySurfaceInfos();
	unordered_map<string, SurfaceMaterialElementInfo>	GetMaterialElementInfos();

	void StoreMaterialParameter(XMFLOAT4& target, const FbxDouble3& srcColor, const FbxDouble& srcFactor = 1.0f);

private:
	FbxSceneInfo									m_fbxSceneInfo;

	unordered_map<string, FbxMeshInfo>				m_fbxMeshes;
	unordered_map<string, FbxSurfaceMaterialInfo>	m_fbxMaterials;
};

template<typename T>
inline void EngineFbxIO::GetMaterialProperty(T& target, FbxSurfaceMaterial* material, const char* name)
{
	FbxProperty materialProperty = material->FindProperty(name);
	if (!materialProperty.IsValid())
		return;

	target = materialProperty.Get<T>();
}

