#pragma once
#include "EngineUtils.h"
#include "EngineTimer.h"
#include "EngineSystem.h"
#include "ResourceManager.h"
#include "SceneManager.h"

class Engine
{
	DECLARE_SINGLE(Engine)

public:
	bool Init(HINSTANCE hInstance, float width, float height, int nCmdShow);
	void Update(const float& deltaTime);
	void Render(const float& deltaTime);
	void PostUpdate(const float& deltaTime);
	
public:
	float AspectRatio();

public:
	WindowInfo	GetWindowInfo()		{ return m_wndInfo; }

private:
	HINSTANCE	m_hInstance;
	WindowInfo	m_wndInfo;

	bool		m_paused;
	float		m_engineTime;
	EngineTimer	m_engineTimer;

public:
	int		Run();
	BOOL	InitWindowInstance(int nCmdShow);
	ATOM	MyRegisterClass(HINSTANCE hInstance);
	LRESULT WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	D3D12_VIEWPORT			 m_viewport;
	D3D12_RECT				 m_scissorRect;

	unique_ptr<Scene>		 m_scene;
};