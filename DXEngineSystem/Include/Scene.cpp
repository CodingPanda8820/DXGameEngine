#include "pch.h"
#include "Scene.h"
#include "EngineSystem.h"

Scene::Scene()
{
	m_observer = make_shared<Camera>();
	m_camera_gameUI = make_shared<Camera>();

	m_lightGroup = make_shared<LightGroup>();

	m_totalTime = 0.0f;

	m_mouseReception.x = 100;
	m_mouseReception.y = 100;

	m_attributes = make_unique<UploadBuffer<CBScene>>(EngineSystem::GetInst()->GetDevice().Get(), 1, 0);
}

Scene::~Scene()
{
}

bool Scene::Init()
{
	InitCamera();
	InitLightGroup();
	InitGameUI();
	InitGeometries();

	SetCameraToObserver("Main");
	SetCameraToRenderGameUI("UICamera");

	return true;
}

void Scene::Update(const float& deltaTime)
{
	UpdateInputAndTimer(deltaTime);
}

void Scene::Render(const float& deltaTime)
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = EngineSystem::GetInst()->GetCmdList();

	RenderInputAndTimer(deltaTime);

	m_observer->Update();
	m_observer->Render();

	m_lightGroup->Update();
	m_lightGroup->Render();

	//	Deferred Rendering
	int currentBackBufferIndex = EngineSystem::GetInst()->GetSWAPCHAIN()->GetCurrentBackBuffer();
	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::SWAP_CHAIN)->ClearRenderTargetView(cmdList, currentBackBufferIndex);
	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::GBUFFER)->ClearRenderTargetViews(cmdList);
	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::GBUFFER)->OMSetRenderTargets(cmdList);
	for (shared_ptr<Geometry> geometry : m_renderLayers_deferred[RENDER_LAYER_ID(RENDER_LAYER::SRGB)])
	{
		geometry->Render();
	}

	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::GBUFFER)->WaitSync(cmdList, WAIT_SYNC_TYPE::RENDER_TARGET_TO_RESOURCE);

	//	Deferred Shadow
	RenderShadow();

	//	Deferred Lighting
	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::LIGHTING)->ClearRenderTargetViews(cmdList);
	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::LIGHTING)->OMSetRenderTargets(cmdList);
	for (auto light : m_lightGroup->GetLights(LIGHT_TYPE::DIRECTIONAL))
	{
		light->Render();
	}

	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::LIGHTING)->WaitSync(cmdList, WAIT_SYNC_TYPE::RENDER_TARGET_TO_RESOURCE);
	EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::SWAP_CHAIN)->OMSetRenderTarget(cmdList, 1, currentBackBufferIndex);
	for (auto light : m_lightGroup->GetLights(LIGHT_TYPE::AMBIENT))
	{
		light->Render();
	}

	//	Forward Rendering
	//for (vector<shared_ptr<Geometry>> renderLayer : m_renderLayers_forward)
	//{
	//	for (shared_ptr<Geometry> geometry : renderLayer)
	//	{
	//		geometry->Render();
	//	}
	//}

	m_camera_gameUI->Update();
	m_camera_gameUI->Render();

	for (shared_ptr<Geometry> geometry : m_renderLayers_gameUI)
	{
		geometry->Render();
	}
}

void Scene::PostUpdate(const float& deltaTime)
{
}

void Scene::InitCamera()
{
#pragma region Camera
	m_cameras["Main"] = make_shared<Camera>(PROJECTION_TYPE::PERSPECTIVE);
	m_cameras["Main"]->Init();
	//m_cameras["Main"]->GetTransform()->SetTranslate(0.0f, 30.0f, 0.0f);
	m_cameras["Main"]->GetTransform()->SetTranslate(0.0f, 0.0f, -40.0f);
	//m_cameras["Main"]->GetTransform()->SetRotate(90.0f, 0.0f, 0.0f);
	m_cameras["Main"]->GetTransform()->SetRotate(0.0f, 0.0f, 0.0f);

	m_cameras["UICamera"] = make_shared<Camera>(PROJECTION_TYPE::ORTHOGONAL);
	m_cameras["UICamera"]->Init();
	m_cameras["UICamera"]->GetTransform()->SetTranslate(0.0f, 0.0f, 0.0f);
#pragma endregion
}

void Scene::InitLightGroup()
{
#pragma region LightGroup
	m_lightGroup->Init();
	m_lightGroup->AddLight("Ambient", LIGHT_TYPE::AMBIENT);
	m_lightGroup->GetLight("Ambient")->SetStrength(0.5f, 0.5f, 0.5f);
	m_lightGroup->AddLight("Directional", LIGHT_TYPE::DIRECTIONAL);
	m_lightGroup->GetLight("Directional")->SetMaterial("MAT_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_lightGroup->GetLight("Directional")->SetStrength(0.48f, 0.48f, 0.48f);
	m_lightGroup->GetLight("Directional")->SetRotate(90.0f, 0.0f, 0.0f);
	m_lightGroup->GetLight("Directional")->SetTranslate(0.0f, 50.0f, 0.0f);
#pragma endregion
}

void Scene::InitGameUI()
{
#pragma region GameUI

	shared_ptr<Texture> tmpTexture;

	for (int i = 0; i < 6; ++i)
	{
		string temp_name_geometries = "GUI_Deferred_Geometry_" + to_string(i);
		string temp_name_polysurface = "PLY_Deferred_Polysurface_" + to_string(i);
		string temp_name_material = "MAT_Deferred_Geometry_" + to_string(i);

		m_geometries[temp_name_geometries] = make_shared<Geometry>();
		m_geometries[temp_name_geometries]->Init();
		m_geometries[temp_name_geometries]->SetShader("SHD_GameUI");
		m_geometries[temp_name_geometries]->AddPolySurface(temp_name_polysurface);
		m_geometries[temp_name_geometries]->GetPolySurface(temp_name_polysurface)->SetMaterial(temp_name_material);
		if (i < 3)
		{
			shared_ptr<Texture> tmpTexture = EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::GBUFFER)->GetRenderTarget(i);
			ResourceManager::GetInst()->FindMaterial(temp_name_material)->SetTexture(0, tmpTexture);
		}
		else if (i >= 3 && i < 5)
		{
			shared_ptr<Texture> tmpTexture = EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::LIGHTING)->GetRenderTarget(i - 3);
			ResourceManager::GetInst()->FindMaterial(temp_name_material)->SetTexture(0, tmpTexture);
		}
		else
		{
			shared_ptr<Texture> tmpTexture = EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::SHADOW)->GetRenderTarget(i - 5);
			ResourceManager::GetInst()->FindMaterial(temp_name_material)->SetTexture(0, tmpTexture);
		}

		m_geometries[temp_name_geometries]->GetTransform()->SetRotate(-90.0f, 0.0f, 0.0f);
		m_geometries[temp_name_geometries]->GetTransform()->SetScale(0.1f, 0.1f, 0.1f);
		m_geometries[temp_name_geometries]->GetTransform()->SetTranslate(-550.0f + (140.0f * i), 300.0f, 500.0f);
		m_renderLayers_gameUI.push_back(m_geometries[temp_name_geometries]);
	}
#pragma endregion
}

void Scene::InitGeometries()
{
#pragma region Geometry
	m_geometries["SkyDome"] = make_shared<Geometry>();
	m_geometries["SkyDome"]->Init();
	m_geometries["SkyDome"]->SetShader("SHD_SkyDome");
	m_geometries["SkyDome"]->SetMaterial("MAT_SkyDome");
	m_geometries["SkyDome"]->AddPolySurface("SkyDome");
	m_renderLayers_forward[RENDER_LAYER_ID(RENDER_LAYER::SKYBOX)].push_back(m_geometries["SkyDome"]);

	m_geometries["WoodBox_01"] = make_shared<Geometry>();
	m_geometries["WoodBox_01"]->Init();
	m_geometries["WoodBox_01"]->SetShader("SHD_SurfaceShader", OBJECT_RENDER_TYPE::OBJECT);
	m_geometries["WoodBox_01"]->SetShader("SHD_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_geometries["WoodBox_01"]->SetMaterial("MAT_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_geometries["WoodBox_01"]->AddPolySurface("Box");
	{
		m_geometries["WoodBox_01"]->GetPolySurface("Box")->SetMaterial("MAT_WoodBox");
		{
			shared_ptr<Texture> texture = ResourceManager::GetInst()->FindTexture("TX_WoodBox");
			ResourceManager::GetInst()->FindMaterial("MAT_WoodBox")->SetTexture(0, texture);
		}
	}
	m_geometries["WoodBox_01"]->GetTransform()->SetTranslate(0.0f, 2.5f, 0.0f);
	m_geometries["WoodBox_01"]->GetTransform()->SetScale(5.0f, 5.0f, 5.0f);
	m_renderLayers_deferred[RENDER_LAYER_ID(RENDER_LAYER::SRGB)].push_back(m_geometries["WoodBox_01"]);

	m_geometries["WoodBox_02"] = make_shared<Geometry>();
	m_geometries["WoodBox_02"]->Init();
	m_geometries["WoodBox_02"]->SetShader("SHD_SurfaceShader");
	m_geometries["WoodBox_02"]->SetShader("SHD_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_geometries["WoodBox_02"]->SetMaterial("MAT_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_geometries["WoodBox_02"]->AddPolySurface("Box");
	{
		m_geometries["WoodBox_02"]->GetPolySurface("Box")->SetMaterial("MAT_WoodBox");
		{
			shared_ptr<Texture> texture = ResourceManager::GetInst()->FindTexture("TX_WoodBox");
			ResourceManager::GetInst()->FindMaterial("MAT_WoodBox")->SetTexture(0, texture);
		}
	}
	m_geometries["WoodBox_02"]->GetTransform()->SetTranslate(7.5f, 2.5f, 0.0f);
	m_geometries["WoodBox_02"]->GetTransform()->SetScale(5.0f, 5.0f, 5.0f);
	m_renderLayers_deferred[RENDER_LAYER_ID(RENDER_LAYER::SRGB)].push_back(m_geometries["WoodBox_02"]);

	m_geometries["Grid"] = make_shared<Geometry>();
	m_geometries["Grid"]->Init();
	m_geometries["Grid"]->SetShader("SHD_SurfaceShader");
	m_geometries["Grid"]->SetShader("SHD_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_geometries["Grid"]->SetMaterial("MAT_Shadow", OBJECT_RENDER_TYPE::SHADOW);
	m_geometries["Grid"]->AddPolySurface("Grid");
	{
		m_geometries["Grid"]->GetPolySurface("Grid")->SetMaterial("MAT_Grid");
		{
			shared_ptr<Texture> texture = ResourceManager::GetInst()->FindTexture("TX_CheckerBoard");
			ResourceManager::GetInst()->FindMaterial("MAT_Grid")->SetTexture(0, texture);
		}
	}
	m_geometries["Grid"]->GetTransform()->SetTranslate(0.0f, 0.0f, 0.0f);
	m_geometries["Grid"]->GetTransform()->SetScale(5.0f, 5.0f, 5.0f);
	m_renderLayers_deferred[RENDER_LAYER_ID(RENDER_LAYER::SRGB)].push_back(m_geometries["Grid"]);
#pragma endregion
}

void Scene::RenderDeferred()
{
}

void Scene::RenderLighting()
{
}

void Scene::RenderShadow()
{
	ComPtr<ID3D12GraphicsCommandList>	cmdList	= EngineSystem::GetInst()->GetCmdList();
	shared_ptr<MultiRenderTarget> rendertargets = EngineSystem::GetInst()->GetMultiRenderTarget(RENDER_TARGET_TYPE::SHADOW);

	//	Deferred Shadow
	rendertargets->ClearRenderTargetViews(cmdList);
	rendertargets->OMSetRenderTargets(cmdList);

	for (auto light : m_lightGroup->GetLights(LIGHT_TYPE::DIRECTIONAL))
	{
		light->Render(OBJECT_RENDER_TYPE::SHADOW);

		for (shared_ptr<Geometry> geometry : m_renderLayers_deferred[RENDER_LAYER_ID(RENDER_LAYER::SRGB)])
		{
			geometry->Render(OBJECT_RENDER_TYPE::SHADOW);
		}
	}

	rendertargets->WaitSync(cmdList, WAIT_SYNC_TYPE::RENDER_TARGET_TO_RESOURCE);
}

void Scene::RenderForward()
{
}

void Scene::RenderGameUI()
{
}

void Scene::SetCameraToObserver(const string& name)
{
	m_observer = m_cameras[name];
}

void Scene::SetCameraToRenderGameUI(const string& name)
{
	m_camera_gameUI = m_cameras[name];
}

void Scene::UpdateInputAndTimer(const float& deltaTime)
{
	m_totalTime += deltaTime;

	UpdateMouseAction(deltaTime);
	UpdateKeyboardAction(deltaTime);

	CBScene attributes;
	attributes.TotalTime = m_totalTime;
	attributes.DeltaTime = deltaTime;

	m_attributes->CopyData(0, attributes);
}

void Scene::RenderInputAndTimer(const float& deltaTime)
{
	RenderTimer();
}

void Scene::UpdateTimer(const float& deltaTime)
{
	m_totalTime += deltaTime;
}

void Scene::RenderTimer()
{
	auto buffer = m_attributes->GetUploadBuffer();
	UINT bufferByteSize = ConvertToMinimumHardwareAllocationByteSize(sizeof(CBScene));
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = buffer->GetGPUVirtualAddress();
	EngineSystem::GetInst()->GetCmdList()->SetGraphicsRootConstantBufferView(REGISTER_ID(CREGISTER_TYPE::SCENE), gpuAddress);
}

void Scene::UpdateMouseAction(const float& deltaTime)
{
	POINT mousePosition = EngineInput::GetInst()->GetMousePosition();
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::MLB))
	{
		float dx = static_cast<float>(mousePosition.x - m_mousePosPrev.x) * m_mouseReception.x * deltaTime * 0.5;
		float dy = static_cast<float>(mousePosition.y - m_mousePosPrev.y) * m_mouseReception.y * deltaTime * 0.5;

		m_observer->Tilt(dy);
		m_observer->Panning(dx);
	}

	m_mousePosPrev = mousePosition;
}

void Scene::UpdateKeyboardAction(const float& deltaTime)
{
	static float test = 0.0f;
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::W))
		m_observer->Dolly(5.0f * deltaTime);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::S))
		m_observer->Dolly(-5.0f * deltaTime);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::A))
		m_observer->Track(-5.0f * deltaTime);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::D))
		m_observer->Track(5.0f * deltaTime);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::UP))
		m_observer->GetTransform()->UpdateTranslate(0.0f, 5.0f * deltaTime, 0.0f);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::DOWN))
		m_observer->GetTransform()->UpdateTranslate(0.0f, -5.0f * deltaTime, 0.0f);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::RIGHT))
		m_observer->GetTransform()->UpdateTranslate(5.0f * deltaTime, 0.0f, 0.0f);
	if (EngineInput::GetInst()->GetButton(KEY_TYPE::LEFT))
		m_observer->GetTransform()->UpdateTranslate(-5.0f * deltaTime, 0.0f, 0.0f);
}