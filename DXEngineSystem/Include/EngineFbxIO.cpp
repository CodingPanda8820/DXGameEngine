#include "pch.h"
#include "EngineFbxIO.h"

EngineFbxIO::EngineFbxIO()
{
}

EngineFbxIO::~EngineFbxIO()
{
}

void EngineFbxIO::Load(const wstring& filePath)
{
	Import(filePath);

	ParseNode(m_fbxScene->GetRootNode());
}

void EngineFbxIO::Import(const wstring& filePath)
{
	//	FBX Manager
	m_fbxManager = FbxManager::Create();

	//	IO Settings
	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(m_fbxManager, IOSROOT);
	m_fbxManager->SetIOSettings(fbxIOSettings);

	//	FBX Scene
	m_fbxScene = FbxScene::Create(m_fbxManager, "");

	//	FBX Importer
	m_fbxImporter = FbxImporter::Create(m_fbxManager, "");

	//	Import FBX file
	string sFilePath = ConvertWStringToString(filePath);
	m_fbxImporter->Initialize(sFilePath.c_str(), -1, m_fbxManager->GetIOSettings());
	m_fbxImporter->Import(m_fbxScene);

	m_fbxScene->GetGlobalSettings().SetAxisSystem(FbxAxisSystem::DirectX);

	//	Convert Mesh to Triangle
	FbxGeometryConverter geometryConverter(m_fbxManager);
	geometryConverter.Triangulate(m_fbxScene, true);

	m_fbxImporter->Destroy();
}

void EngineFbxIO::ParseNode(FbxNode* node)
{
	string name = node->GetName();
	FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

	vector<FbxSurfaceMaterialInfo> localMaterialInfos;

	//	Load Material
	const int materialCount = node->GetMaterialCount();
	for (uint32 i = 0; i < materialCount; ++i)
	{
		FbxSurfaceMaterial* surfaceMaterial = node->GetMaterial(i);

		FbxSurfaceMaterialInfo surfaceMaterialInfo = LoadMaterial(surfaceMaterial);

		m_fbxMaterials[surfaceMaterialInfo.Name] = surfaceMaterialInfo;
		localMaterialInfos.push_back(surfaceMaterialInfo);

		m_fbxSceneInfo.Materials.push_back(name);
	}

	//	Load Mesh
	if (nodeAttribute)
	{
		switch (nodeAttribute->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			m_fbxMeshes[name] = LoadMesh(name, node->GetMesh(), localMaterialInfos);

			m_fbxSceneInfo.Meshes.push_back(name);

			break;
		}
	}

	//	Recursion Tree
	const int32 childCount = node->GetChildCount();
	for (int32 i = 0; i < childCount; ++i)
		ParseNode(node->GetChild(i));
}

FbxMeshInfo EngineFbxIO::LoadMesh(const string& nodeName, FbxMesh* fbxMesh,
								vector<FbxSurfaceMaterialInfo> localMaterials)
{
	FbxMeshInfo meshInfo = {};
	meshInfo.Name = nodeName;

	//	Get ControlPoints
	FbxVector4* controlPoints = fbxMesh->GetControlPoints();
	const int32 controlPointsCount = fbxMesh->GetControlPointsCount();
	for (int i = 0; i < controlPointsCount; ++i)
	{
		Point pt;
		pt.X = controlPoints[i].mData[0];
		pt.Y = controlPoints[i].mData[2];
		pt.Z = controlPoints[i].mData[1];

		meshInfo.Points.push_back(pt);
	}

	//	Create Indices by material assigned to polySurface
	FbxGeometryElementMaterial* geometryElementMaterial = fbxMesh->GetElementMaterial();
	unordered_map<string, vector<uint32>> IndicesByMaterial;

	//	Get Vertices And Indices
	uint32 currentIndex = 0;
	const uint32 polyCounts = fbxMesh->GetPolygonCount();
	for (int i = 0; i < polyCounts; i++)
	{
		uint32 LocalMaterialIndex = geometryElementMaterial->GetIndexArray().GetAt(i);
		string materialName = localMaterials[LocalMaterialIndex].Name;

		uint32 polySurfaceIndices[3];
		for (int j = 0; j < 3; j++)
		{
			uint32 pointIndex = fbxMesh->GetPolygonVertex(i, j);

			Vertex vertex		= {};
			uint32 vertexIndex	= 0;

			GetVertexPoint(meshInfo.Points[pointIndex], &vertex);
			GetVertexNormal(fbxMesh, &vertex, currentIndex, pointIndex);
			GetVertexTangent(fbxMesh, &vertex, currentIndex, pointIndex);
			GetVertexUV(fbxMesh, &vertex, currentIndex, fbxMesh->GetTextureUVIndex(i, j));

			meshInfo.Vertices.push_back(vertex);
			polySurfaceIndices[j] = currentIndex;

			currentIndex++;
		}

		IndicesByMaterial[materialName].push_back(polySurfaceIndices[0]);
		IndicesByMaterial[materialName].push_back(polySurfaceIndices[2]);
		IndicesByMaterial[materialName].push_back(polySurfaceIndices[1]);
	}

	for (pair<string, vector<uint32>> iter : IndicesByMaterial)
	{

		//	Define PolySurfaceInfo
		FbxMeshShapeInfo meshShapeInfo;
		meshShapeInfo.MaterialName = iter.first;
		meshShapeInfo.IndexCount = iter.second.size();
		meshShapeInfo.StartIndexLocation = meshInfo.Indices.size();

		//	Merge Indices
		for (int i = 0; i < iter.second.size(); i++)
			meshInfo.Indices.push_back(iter.second[i]);

		meshInfo.MeshShapeInfo.push_back(meshShapeInfo);
	}

	return meshInfo;
}

FbxSurfaceMaterialInfo EngineFbxIO::LoadMaterial(FbxSurfaceMaterial* fbxMaterial)
{
	FbxSurfaceMaterialInfo materialInfo;
	materialInfo.Name = fbxMaterial->GetName();

	GetMaterialProperty(materialInfo.Diffuse, fbxMaterial, FbxSurfaceMaterial::sDiffuse);
	GetMaterialProperty(materialInfo.DiffuseFactor, fbxMaterial, FbxSurfaceMaterial::sDiffuseFactor);
	GetMaterialProperty(materialInfo.Specular, fbxMaterial, FbxSurfaceMaterial::sSpecular);
	GetMaterialProperty(materialInfo.SpecularFactor, fbxMaterial, FbxSurfaceMaterial::sSpecularFactor);
	GetMaterialProperty(materialInfo.Ambient, fbxMaterial, FbxSurfaceMaterial::sAmbient);
	GetMaterialProperty(materialInfo.AmbientFactor, fbxMaterial, FbxSurfaceMaterial::sAmbientFactor);
	GetMaterialProperty(materialInfo.Shininess, fbxMaterial, FbxSurfaceMaterial::sShininess);

	GetMaterialTexturePath(materialInfo.TexturePaths["Diffuse"], fbxMaterial, FbxSurfaceMaterial::sDiffuse);
	GetMaterialTexturePath(materialInfo.TexturePaths["Specular"], fbxMaterial, FbxSurfaceMaterial::sSpecular);
	GetMaterialTexturePath(materialInfo.TexturePaths["Bump"], fbxMaterial, FbxSurfaceMaterial::sBump);
	GetMaterialTexturePath(materialInfo.TexturePaths["Normal"], fbxMaterial, FbxSurfaceMaterial::sNormalMap);
	GetMaterialTexturePath(materialInfo.TexturePaths["Shininess"], fbxMaterial, FbxSurfaceMaterial::sShininess);

	return materialInfo;
}

void EngineFbxIO::GetVertexPoint(Point point, Vertex* vertex)
{
	vertex->Position.x = point.X;
	vertex->Position.y = point.Y;
	vertex->Position.z = point.Z;
}

void EngineFbxIO::GetVertexNormal(FbxMesh* mesh, Vertex* vertex, int32 index, int32 pointIndex)
{
	if (mesh->GetElementNormalCount() == 0)
		return;

	FbxGeometryElementNormal* normal = mesh->GetElementNormal();

	uint32 normalIndex = 0;
	switch (normal->GetMappingMode())
	{
	case FbxGeometryElement::eByPolygonVertex:
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			normalIndex = pointIndex;
		else
			normalIndex = normal->GetIndexArray().GetAt(pointIndex);
	}
	break;
	case FbxGeometryElement::eByControlPoint:
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
			normalIndex = index;
		else
			normalIndex = normal->GetIndexArray().GetAt(index);
	}
	default:
		break;
	}

	FbxVector4 N = normal->GetDirectArray().GetAt(normalIndex);
	vertex->Normal.x = static_cast<float>(N.mData[0]);
	vertex->Normal.y = static_cast<float>(N.mData[2]);
	vertex->Normal.z = static_cast<float>(N.mData[1]);
}

void EngineFbxIO::GetVertexTangent(FbxMesh* mesh, Vertex* vertex, int32 index, int32 pointIndex)
{
	if (mesh->GetElementTangentCount() == 0)
		return;

	FbxGeometryElementTangent* tangent = mesh->GetElementTangent();

	uint32 elementIndex = 0;
	switch (tangent->GetMappingMode())
	{
	case FbxGeometryElement::eByPolygonVertex:
	{
		if (tangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			elementIndex = index;
		else
			elementIndex = tangent->GetIndexArray().GetAt(index);
	}
	case FbxGeometryElement::eByControlPoint:
	{
		if (tangent->GetReferenceMode() == FbxGeometryElement::eDirect)
			elementIndex = pointIndex;
		else
			elementIndex = tangent->GetIndexArray().GetAt(pointIndex);
	}
	default:
		return;
	}
	
	FbxVector4 T = tangent->GetDirectArray().GetAt(elementIndex);
	vertex->TangentU.x = static_cast<float>(T.mData[0]);
	vertex->TangentU.y = static_cast<float>(T.mData[2]);
	vertex->TangentU.z = static_cast<float>(T.mData[1]);
}

void EngineFbxIO::GetVertexUV(FbxMesh* mesh, Vertex* vertex, int32 index, int32 uvIndex)
{
	FbxVector2 UV = mesh->GetElementUV()->GetDirectArray().GetAt(uvIndex);
	vertex->UV.x = static_cast<float>(UV.mData[0]);
	vertex->UV.y = 1.0f - static_cast<float>(UV.mData[1]);
}

void EngineFbxIO::GetMaterialTexturePath(wstring& filePath, FbxSurfaceMaterial* material, const char* name)
{
	FbxProperty textureProperty = material->FindProperty(name);

	if (!textureProperty.IsValid() || !textureProperty.GetSrcObject())
	{
		filePath = L"";
		return;
	}

	FbxFileTexture* texture = textureProperty.GetSrcObject<FbxFileTexture>(0);

	if (!texture)
	{
		filePath = L"";
		return;
	}
	
	wstring textureRelativeFilePath = ConvertStringToWString(texture->GetRelativeFileName());

	filePath =  L"D:\\Code_CodingPanda\\DXEngine\\DXEngineResource\\Asset\\" + textureRelativeFilePath;
}

FbxSceneInfo EngineFbxIO::GetFBXSceneInfo()
{
	return m_fbxSceneInfo;
}

unordered_map<string, FbxMeshInfo> EngineFbxIO::GetFBXMeshes()
{
	return m_fbxMeshes;
}

unordered_map<string, FbxSurfaceMaterialInfo> EngineFbxIO::GetFBXMaterials()
{
	return m_fbxMaterials;
}

unordered_map<string, PolySurfaceInfo> EngineFbxIO::GetPolySurfaceInfos()
{
	unordered_map<string, PolySurfaceInfo> polySurfaceInfos;

	for (pair<string, FbxMeshInfo> meshInfo : m_fbxMeshes)
	{
		PolySurfaceInfo polySurfaceInfo = {};
		polySurfaceInfo.name	 = meshInfo.second.Name;
		polySurfaceInfo.Vertices = meshInfo.second.Vertices;
		polySurfaceInfo.Indices	 = meshInfo.second.Indices;

		for (FbxMeshShapeInfo meshShapeInfo : meshInfo.second.MeshShapeInfo)
		{
			PolySurfaceShape surfaceShapeInfo = {};
			surfaceShapeInfo.MaterialName		 = meshShapeInfo.MaterialName;
			surfaceShapeInfo.IndexCount			 = meshShapeInfo.IndexCount;
			surfaceShapeInfo.StartIndexLocation  = meshShapeInfo.StartIndexLocation;
			surfaceShapeInfo.StartVertexLocation = meshShapeInfo.StartVertexLocation;

			polySurfaceInfo.PolySurfaceShapes.push_back(surfaceShapeInfo);
		}

		polySurfaceInfos[polySurfaceInfo.name] = polySurfaceInfo;
	}

	return polySurfaceInfos;
}

unordered_map<string, SurfaceMaterialElementInfo> EngineFbxIO::GetMaterialElementInfos()
{
	unordered_map<string, SurfaceMaterialElementInfo> materialElements = {};

	for (pair<string, FbxSurfaceMaterialInfo> fbxMaterial : m_fbxMaterials)
	{
		SurfaceMaterialElementInfo surfaceMaterial = {};

		surfaceMaterial.Name = fbxMaterial.second.Name;

		StoreMaterialParameter(surfaceMaterial.Ambient, fbxMaterial.second.Ambient, fbxMaterial.second.AmbientFactor);
		StoreMaterialParameter(surfaceMaterial.Diffuse, fbxMaterial.second.Diffuse, fbxMaterial.second.DiffuseFactor);
		StoreMaterialParameter(surfaceMaterial.Specular, fbxMaterial.second.Specular, fbxMaterial.second.SpecularFactor);
		StoreMaterialParameter(surfaceMaterial.Shininess, fbxMaterial.second.Shininess);

		surfaceMaterial.DiffuseTexturePath	 = fbxMaterial.second.TexturePaths["Diffuse"];
		surfaceMaterial.SpecularTexturePath	 = fbxMaterial.second.TexturePaths["Specular"];
		surfaceMaterial.AmbientTexturePath	 = fbxMaterial.second.TexturePaths["Ambient"];
		surfaceMaterial.ShininessTexturePath = fbxMaterial.second.TexturePaths["Shiniess"];
		surfaceMaterial.NormalTexturePath	 = fbxMaterial.second.TexturePaths["Normal"];

		materialElements[surfaceMaterial.Name] = surfaceMaterial;
	}

	return materialElements;
}

void EngineFbxIO::StoreMaterialParameter(XMFLOAT4& target, const FbxDouble3& srcColor, const FbxDouble& srcFactor)
{
	target.x = srcColor.mData[0];
	target.y = srcColor.mData[1];
	target.z = srcColor.mData[2];
	target.w = srcFactor;
}
