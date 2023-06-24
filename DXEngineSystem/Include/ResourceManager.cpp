#include "pch.h"
#include "EngineSystem.h"
#include "ResourceManager.h"
#include "EngineFbxIO.h"

DEFINITION_SINGLE(ResourceManager)

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{

}

bool ResourceManager::Init()
{
	static float EngineWidth  = EngineSystem::GetInst()->GetWindowInfo().Width;
	static float EngineHeight = EngineSystem::GetInst()->GetWindowInfo().Height;

#pragma region Textures
	LoadTextureFile("TX_WoodBox", L"..\\..\\DXEngineResource\\Texture\\Box.jpg");
	LoadTextureFile("TX_SkyDome", L"..\\..\\DXEngineResource\\Texture\\SkyDome.jpg");
	LoadTextureFile("TX_CheckerBoard", L"..\\..\\DXEngineResource\\Texture\\CheckerBoard\\CheckerBoard_Type01_1k.png");
#pragma endregion

#pragma region FBX File
	//ImportFBXFile(device, cmdList, "DragonWarrior", L"..\\..\\DXEngineResource\\Asset\\Little_DragonWarior.fbx");
#pragma endregion

#pragma region Default Shader

	ShaderInfo shaderInfo =
	{
		SHADER_TYPE::DEFERRED
	};

	NewShader("SHD_SurfaceShader");
	m_shaders["SHD_SurfaceShader"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, 0, 0);
	m_shaders["SHD_SurfaceShader"]->AppendInputElement("NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, 0, 0);
	m_shaders["SHD_SurfaceShader"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_SurfaceShader"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_SurfaceShader"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredShader.hlsli", "VS_Main", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_SurfaceShader"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredShader.hlsli", "PS_Main", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_SurfaceShader"]->Create("SHD_SurfaceShader", shaderInfo);

	shaderInfo =
	{
		SHADER_TYPE::FORWARD
	};

	NewShader("SHD_ForwardShader");
	m_shaders["SHD_ForwardShader"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_ForwardShader"]->AppendInputElement("NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_ForwardShader"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_ForwardShader"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_ForwardShader"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\ForwardShader.hlsli", "VS_Main", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_ForwardShader"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\ForwardShader.hlsli", "PS_Main", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_ForwardShader"]->Create("SHD_ForwardShader", shaderInfo);

	shaderInfo =
	{
		SHADER_TYPE::LIGHTING,
		RASTERIZER_TYPE::CULL_NONE,
		DEPTH_STENCIL_TYPE::DEPTH_UNABLE_NOT_WRITE,
		BLEND_TYPE::ADD
	};

	NewShader("SHD_AmbientLight");
	m_shaders["SHD_AmbientLight"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_AmbientLight"]->AppendInputElement("NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_AmbientLight"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_AmbientLight"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_AmbientLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "VS_AmbientLight", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_AmbientLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "PS_AmbientLight", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_AmbientLight"]->Create("SHD_AmbientLight", shaderInfo);

	NewShader("SHD_DirectionalLight");
	m_shaders["SHD_DirectionalLight"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_DirectionalLight"]->AppendInputElement("NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_DirectionalLight"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_DirectionalLight"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_DirectionalLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "VS_DirectionalLight", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_DirectionalLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "PS_DirectionalLight", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_DirectionalLight"]->Create("SHD_DirectionalLight", shaderInfo);

	NewShader("SHD_SpotLight");
	m_shaders["SHD_SpotLight"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_SpotLight"]->AppendInputElement("NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_SpotLight"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_SpotLight"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_SpotLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "VS_DirectionalLight", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_SpotLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "PS_DirectionalLight", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_SpotLight"]->Create("SHD_SpotLight", shaderInfo);

	NewShader("SHD_PointLight");
	m_shaders["SHD_PointLight"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_PointLight"]->AppendInputElement("NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_PointLight"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_PointLight"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_PointLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "VS_DirectionalLight", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_PointLight"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\DeferredLighting.hlsli", "PS_DirectionalLight", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_PointLight"]->Create("SHD_PointLight", shaderInfo);

	shaderInfo =
	{
		SHADER_TYPE::FORWARD
	};

	NewShader("SHD_GameUI");
	m_shaders["SHD_GameUI"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_GameUI"]->AppendInputElement("NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_GameUI"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_GameUI"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_GameUI"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\GameUI.hlsli", "VS_Main", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_GameUI"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\GameUI.hlsli", "PS_Main", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_GameUI"]->Create("SHD_GameUI", shaderInfo);

	shaderInfo =
	{
		SHADER_TYPE::FORWARD,
		RASTERIZER_TYPE::CULL_NONE,
		DEPTH_STENCIL_TYPE::LESS_EQUAL
	};

	NewShader("SHD_SkyDome");
	m_shaders["SHD_SkyDome"]->AppendInputElement("POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0,  0, 0, 0);
	m_shaders["SHD_SkyDome"]->AppendInputElement("NORMAL",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, 0, 0);
	m_shaders["SHD_SkyDome"]->AppendInputElement("TANGENT",	0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, 0, 0);
	m_shaders["SHD_SkyDome"]->AppendInputElement("TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 36, 0, 0);
	m_shaders["SHD_SkyDome"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\SkyBox.hlsli", "VS_Main", SHADER_CODE_TYPE::VS);
	m_shaders["SHD_SkyDome"]->LoadShaderByteCode(L"..\\..\\DXEngineResource\\Shader\\SkyBox.hlsli", "PS_Main", SHADER_CODE_TYPE::PS);
	m_shaders["SHD_SkyDome"]->Create("SHD_SkyDome", shaderInfo);
#pragma endregion

#pragma region Defulat Material
	NewMaterial("MAT_StandardSurface");
	m_materials["MAT_StandardSurface"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_StandardSurface"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_StandardSurface"]->SetShininess(0.8f, 0.8f, 0.8f, 0.8f);
	m_materials["MAT_StandardSurface"]->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);

	NewMaterial("MAT_CheckerBoard");
	m_materials["MAT_CheckerBoard"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_CheckerBoard"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_CheckerBoard"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_CheckerBoard"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_CheckerBoard"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["TX_CheckerBoard"]);

	NewMaterial("MAT_Deferred_Geometry_0");
	m_materials["MAT_Deferred_Geometry_0"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_0"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_Deferred_Geometry_0"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_0"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);

	NewMaterial("MAT_Deferred_Geometry_1");
	m_materials["MAT_Deferred_Geometry_1"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_1"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_Deferred_Geometry_1"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_1"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);

	NewMaterial("MAT_Deferred_Geometry_2");
	m_materials["MAT_Deferred_Geometry_2"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_2"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_Deferred_Geometry_2"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_2"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);

	NewMaterial("MAT_Deferred_Geometry_3");
	m_materials["MAT_Deferred_Geometry_3"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_3"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_Deferred_Geometry_3"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_3"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);

	NewMaterial("MAT_Deferred_Geometry_4");
	m_materials["MAT_Deferred_Geometry_4"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_4"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_Deferred_Geometry_4"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Deferred_Geometry_4"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);

	NewMaterial("MAT_SkyDome");
	m_materials["MAT_SkyDome"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_SkyDome"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_SkyDome"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_SkyDome"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_SkyDome"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["TX_SkyDome"]);

	NewMaterial("MAT_AmbientLight");
	m_materials["MAT_AmbientLight"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_AmbientLight"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_AmbientLight"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_AmbientLight"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_AmbientLight"]->SetUserDataInt(0, static_cast<uint32>(LIGHT_TYPE::AMBIENT));
	m_materials["MAT_AmbientLight"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D,  m_textures["DRT_Diffuse"]);
	m_materials["MAT_AmbientLight"]->SetTexture(TREGISTER_TYPE::SPECULAR_TEXTURE2D, m_textures["DRT_DiffuseLight"]);
	m_materials["MAT_AmbientLight"]->SetTexture(TREGISTER_TYPE::AMBIENT_TEXTURE2D,  m_textures["DRT_SpecularLight"]);

	NewMaterial("MAT_DirectionalLight");
	m_materials["MAT_DirectionalLight"]->SetAmbient(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_DirectionalLight"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_DirectionalLight"]->SetShininess(0.0f, 0.0f, 0.0f, 1.0f);
	m_materials["MAT_DirectionalLight"]->SetSpecular(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_DirectionalLight"]->SetUserDataInt(0, static_cast<uint32>(LIGHT_TYPE::DIRECTIONAL));
	m_materials["MAT_DirectionalLight"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["DRT_Position"]);
	m_materials["MAT_DirectionalLight"]->SetTexture(TREGISTER_TYPE::NORMAL_TEXTURE2D,  m_textures["DRT_Normal"]);

	NewMaterial("MAT_SpotLight");
	m_materials["MAT_SpotLight"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_SpotLight"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_SpotLight"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_SpotLight"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_SpotLight"]->SetUserDataInt(0, static_cast<uint32>(LIGHT_TYPE::DIRECTIONAL));
	m_materials["MAT_SpotLight"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["DRT_Position"]);
	m_materials["MAT_SpotLight"]->SetTexture(TREGISTER_TYPE::NORMAL_TEXTURE2D,  m_textures["DRT_Normal"]);

	NewMaterial("MAT_PointLight");
	m_materials["MAT_PointLight"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_PointLight"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_PointLight"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_PointLight"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_PointLight"]->SetUserDataInt(0, static_cast<uint32>(LIGHT_TYPE::DIRECTIONAL));
	m_materials["MAT_PointLight"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["DRT_Position"]);
	m_materials["MAT_PointLight"]->SetTexture(TREGISTER_TYPE::NORMAL_TEXTURE2D,  m_textures["DRT_Normal"]);

	NewMaterial("MAT_WoodBox");
	m_materials["MAT_WoodBox"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_WoodBox"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_WoodBox"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_WoodBox"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_WoodBox"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["TX_WoodBox"]);

	NewMaterial("MAT_Grid");
	m_materials["MAT_Grid"]->SetAmbient(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Grid"]->SetDiffuse(1.0f, 1.0f, 1.0f, 1.0f);
	m_materials["MAT_Grid"]->SetShininess(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Grid"]->SetSpecular(0.0f, 0.0f, 0.0f, 0.0f);
	m_materials["MAT_Grid"]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures["TX_CheckerBoard"]);
#pragma endregion

#pragma region Default PolySurfaces
	NewPolySurface("SkyDome");
	m_polySurfaces["SkyDome"]->CreateSphere(1, 32, 32, "MAT_SkyDome");
	m_polySurfaces["SkyDome"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["SkyDome"]->Commit();

	NewPolySurface("EngineWindow");
	m_polySurfaces["EngineWindow"]->CreateGrid(EngineWidth, EngineHeight, 2, 2);
	m_polySurfaces["EngineWindow"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["EngineWindow"]->Commit();

	NewPolySurface("PLY_Deferred_Polysurface_0");
	m_polySurfaces["PLY_Deferred_Polysurface_0"]->CreateGrid(EngineWidth, EngineHeight, 2, 2);
	m_polySurfaces["PLY_Deferred_Polysurface_0"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["PLY_Deferred_Polysurface_0"]->Commit();

	NewPolySurface("PLY_Deferred_Polysurface_1");
	m_polySurfaces["PLY_Deferred_Polysurface_1"]->CreateGrid(EngineWidth, EngineHeight, 2, 2);
	m_polySurfaces["PLY_Deferred_Polysurface_1"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["PLY_Deferred_Polysurface_1"]->Commit();

	NewPolySurface("PLY_Deferred_Polysurface_2");
	m_polySurfaces["PLY_Deferred_Polysurface_2"]->CreateGrid(EngineWidth, EngineHeight, 2, 2);
	m_polySurfaces["PLY_Deferred_Polysurface_2"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["PLY_Deferred_Polysurface_2"]->Commit();

	NewPolySurface("PLY_Deferred_Polysurface_3");
	m_polySurfaces["PLY_Deferred_Polysurface_3"]->CreateGrid(EngineWidth, EngineHeight, 2, 2);
	m_polySurfaces["PLY_Deferred_Polysurface_3"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["PLY_Deferred_Polysurface_3"]->Commit();

	NewPolySurface("PLY_Deferred_Polysurface_4");
	m_polySurfaces["PLY_Deferred_Polysurface_4"]->CreateGrid(EngineWidth, EngineHeight, 2, 2);
	m_polySurfaces["PLY_Deferred_Polysurface_4"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["PLY_Deferred_Polysurface_4"]->Commit();

	NewPolySurface("UIBaseShape");
	m_polySurfaces["UIBaseShape"]->CreateGrid(256, 256, 2, 2, "MAT_CheckerBoard");
	m_polySurfaces["UIBaseShape"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["UIBaseShape"]->Commit();

	NewPolySurface("Grid");
	m_polySurfaces["Grid"]->CreateGrid(10, 10, 2, 2);
	m_polySurfaces["Grid"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["Grid"]->Commit();

	NewPolySurface("Sphere");
	m_polySurfaces["Sphere"]->CreateSphere(1, 32, 32);
	m_polySurfaces["Sphere"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["Sphere"]->Commit();

	NewPolySurface("Box");
	m_polySurfaces["Box"]->CreateBox(1, 1, 1, 1);
	m_polySurfaces["Box"]->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_polySurfaces["Box"]->Commit();
#pragma endregion

	return true;
}

void ResourceManager::NewTexture(const string& name, ComPtr<ID3D12Device> device, DXGI_FORMAT format,
								uint32 width, uint32 height, const D3D12_HEAP_PROPERTIES& heapProperty,
								D3D12_HEAP_FLAGS heapFlags, D3D12_RESOURCE_FLAGS resourceFlags, XMFLOAT4 clearColor)
{
	shared_ptr<Texture> newTexture = make_shared<Texture>();
	newTexture->SetName(name);
	newTexture->Create(device, format, width, height, heapProperty, heapFlags, resourceFlags, clearColor);

	m_textures[name] = newTexture;
}

void ResourceManager::NewTexture(const string& name, ComPtr<ID3D12Device> device, ComPtr<ID3D12Resource> resource)
{
	shared_ptr<Texture> newTexture = make_shared<Texture>();
	newTexture->SetName(name);
	newTexture->LoadGPUResource(device, resource);

	m_textures[name] = newTexture;
}

void ResourceManager::LoadTextureFile(const string& name, const wstring& filePath)
{
	shared_ptr<Texture> newTexture = make_shared<Texture>();
	newTexture->SetName(name);
	newTexture->LoadTextureFile(filePath, EngineSystem::GetInst()->GetDevice(), EngineSystem::GetInst()->GetCmdList());
	m_textures[name] = newTexture;
}


void ResourceManager::NewShader(const string& name)
{
	shared_ptr<Shader> newShader = make_shared<Shader>();
	m_shaders[name] = newShader;
}

void ResourceManager::NewPolySurface(const string& name)
{
	shared_ptr<PolySurface> newPolySurface = make_shared<PolySurface>();
	newPolySurface->Create(name);
	m_polySurfaces[name] = newPolySurface;
}

void ResourceManager::ImportFBXFile(ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> cmdList,
									const string& assetName, const wstring& filePath)
{
#pragma region ImportFBXFile
	EngineFbxIO* fbxIO = new EngineFbxIO;
	fbxIO->Load(filePath);

	auto materialElementInfos = fbxIO->GetMaterialElementInfos();
	for (pair<string, SurfaceMaterialElementInfo> materialElementInfo : materialElementInfos)
	{
		//	Create Material
		string materialName = materialElementInfo.first;

		NewMaterial(materialName);
		m_materials[materialName]->SetName(materialElementInfo.second.Name);
		m_materials[materialName]->SetDiffuse(materialElementInfo.second.Diffuse);
		m_materials[materialName]->SetSpecular(materialElementInfo.second.Specular);
		m_materials[materialName]->SetAmbient(materialElementInfo.second.Ambient);
		m_materials[materialName]->SetShininess(materialElementInfo.second.Shininess);

		//	Create Texture
		string temp_textureName = "";
		if (materialElementInfo.second.DiffuseTexturePath != L"")
		{
			wstring test = materialElementInfo.second.DiffuseTexturePath;
			temp_textureName = materialName + "_Diffuse";
			LoadTextureFile(temp_textureName, materialElementInfo.second.DiffuseTexturePath);
			m_materials[materialName]->SetTexture(TREGISTER_TYPE::DIFFUSE_TEXTURE2D, m_textures[temp_textureName]);
		}
		if (materialElementInfo.second.SpecularTexturePath != L"")
		{
			temp_textureName = materialName + "_Specular";
			LoadTextureFile(materialName + "_Specular", materialElementInfo.second.SpecularTexturePath);
			m_materials[materialName]->SetTexture(TREGISTER_TYPE::SPECULAR_TEXTURE2D, m_textures[temp_textureName]);
		}
		if (materialElementInfo.second.AmbientTexturePath != L"")
		{
			temp_textureName = materialName + "_Ambient";
			LoadTextureFile(materialName + "_Ambient", materialElementInfo.second.AmbientTexturePath);
			m_materials[materialName]->SetTexture(TREGISTER_TYPE::AMBIENT_TEXTURE2D, m_textures[temp_textureName]);
		}
		if (materialElementInfo.second.ShininessTexturePath != L"")
		{
			temp_textureName = materialName + "_Shininess";
			LoadTextureFile(materialName + "_Shininess", materialElementInfo.second.ShininessTexturePath);
			m_materials[materialName]->SetTexture(TREGISTER_TYPE::SHININESS_TEXTURE2D, m_textures[temp_textureName]);
		}
		if (materialElementInfo.second.NormalTexturePath != L"")
		{
			temp_textureName = materialName + "_Normal";
			LoadTextureFile(materialName + "_Normal", materialElementInfo.second.NormalTexturePath);
			m_materials[materialName]->SetTexture(TREGISTER_TYPE::NORMAL_TEXTURE2D, m_textures[temp_textureName]);
		}
	}

	auto polySurfaceInfos = fbxIO->GetPolySurfaceInfos();
	for (pair<string, PolySurfaceInfo> polySurfaceInfo : polySurfaceInfos)
	{
		//	Create PolySurface
		string polySurfaceName = polySurfaceInfo.second.name;
		NewPolySurface(polySurfaceName);
		m_polySurfaces[polySurfaceName]->SetVertices(polySurfaceInfo.second.Vertices);
		m_polySurfaces[polySurfaceName]->SetIndices(polySurfaceInfo.second.Indices);
		
		for (PolySurfaceShape polySurfaceShape : polySurfaceInfo.second.PolySurfaceShapes)
		{
			//m_polySurfaces[polySurfaceName]->AddPolySurfaceShapeInfos(polySurfaceShapeInfo);
		}

		m_polySurfaces[polySurfaceName]->Commit();
	}

	m_assetInfos[assetName] = make_shared<AssetInfo>();
	m_assetInfos[assetName]->PolySurfaces = fbxIO->GetFBXSceneInfo().Meshes;
	m_assetInfos[assetName]->Materials = fbxIO->GetFBXSceneInfo().Materials;

	SAFE_DELETE(fbxIO);

#pragma endregion
}

void ResourceManager::NewMaterial(const string& name)
{
	m_materials[name] = make_shared<Material>();
	m_materials[name]->SetName(name);
}

void ResourceManager::NewOBJFileData(const string& name, const wstring& filePath, ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCmdList)
{
	NewMaterial(name);
	NewPolySurface(name);

	vector<Vertex> vertices;
	vector<uint32> indices;

	EngineObjIO::LoadOBJFile(filePath, vertices, indices);

	m_polySurfaces[name]->SetVertices(vertices);
	m_polySurfaces[name]->SetIndices(indices);
	m_polySurfaces[name]->Commit();
}


shared_ptr<Shader> ResourceManager::FindShader(const string& name)
{
	return m_shaders[name];
}

shared_ptr<Texture> ResourceManager::FindTexture(const string& name)
{
	return m_textures[name];
}

shared_ptr<Material> ResourceManager::FindMaterial(const string& name)
{
	return m_materials[name];
}

shared_ptr<PolySurface> ResourceManager::FindPolySurface(const string& name)
{
	return m_polySurfaces[name];
}

shared_ptr<AssetInfo> ResourceManager::FindAssetInfo(const string& name)
{
	return m_assetInfos[name];
}