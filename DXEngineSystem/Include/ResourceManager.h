#pragma once
#include "EngineMath.h"
#include "EngineUtils.h"
#include "EngineObjIO.h"
#include "Shader.h"
#include "Material.h"
#include "Texture.h"
#include "Transform.h"
#include "PolySurface.h"

class ResourceManager
{
	DECLARE_SINGLE(ResourceManager)

public:
	bool Init();

public:
	void NewTexture(const string& name, ComPtr<ID3D12Device> device, DXGI_FORMAT format,
					uint32 width, uint32 height, const D3D12_HEAP_PROPERTIES& heapProperty,
					D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resourceFlags,
					XMFLOAT4 clearColor = XMFLOAT4());
	void NewTexture(const string& name, ComPtr<ID3D12Device> device, ComPtr<ID3D12Resource> resource);
	void LoadTextureFile(const string& name, const wstring& filePath);

	void NewShader(const string& name);
	void NewMaterial(const string& name);
	void NewPolySurface(const string& name);
	void NewOBJFileData(const string& name, const wstring& filePath, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList);
	void ImportFBXFile(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> cmdList, const string& assetName, const wstring& filePath);

public:
	shared_ptr<Shader>			FindShader(const string& name);
	shared_ptr<Texture>			FindTexture(const string& name);
	shared_ptr<Material>		FindMaterial(const string& name);
	shared_ptr<PolySurface>		FindPolySurface(const string& name);
	shared_ptr<AssetInfo>		FindAssetInfo(const string& name);

private:
	unordered_map<string, shared_ptr<AssetInfo>>		m_assetInfos;

	unordered_map<string, shared_ptr<Shader>>			m_shaders;
	unordered_map<string, shared_ptr<Texture>>			m_textures;
	unordered_map<string, shared_ptr<Material>>			m_materials;
	unordered_map<string, shared_ptr<PolySurface>>		m_polySurfaces;
};

