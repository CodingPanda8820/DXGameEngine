#include "pch.h"
#include "Engine.h"
#include "Device.h"
#include "CommandObjects.h"
#include "SwapChain.h"
#include "EngineInput.h"

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return Engine::GetInst()->WndProc(hWnd, message, wParam, lParam);
}

DEFINITION_SINGLE(Engine)

Engine::Engine()
{
}

Engine::~Engine()
{

}

bool Engine::Init(HINSTANCE hInstance, float width, float height, int nCmdShow)
{
	m_hInstance = hInstance;

	m_wndInfo.Width = width;
	m_wndInfo.Height = height;
	m_wndInfo.WindowMode = nCmdShow;

	m_viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	m_scissorRect = { 0, 0, static_cast<int>(width), static_cast<int>(height) };

	MyRegisterClass(hInstance);
	InitWindowInstance(true);

	if (!EngineInput::GetInst()->Init(m_wndInfo))
		return false;

	if (!EngineSystem::GetInst()->Init(m_wndInfo))
		return false;

	if (!ResourceManager::GetInst()->Init())
		return false;

	if (!SceneManager::GetInst()->Init())
		return false;

	EngineSystem::GetInst()->CommitCommandLists();
	EngineSystem::GetInst()->FlushCommandQueue();

	return true;
}

void Engine::Update(const float& deltaTime)
{
	EngineInput::GetInst()->Update();

	EngineSystem::GetInst()->Update();

	SceneManager::GetInst()->Update(deltaTime);
}

void Engine::Render(const float& deltaTime)
{
	EngineSystem::GetInst()->Render(m_viewport, m_scissorRect);

	SceneManager::GetInst()->Render(deltaTime);
}

void Engine::PostUpdate(const float& deltaTime)
{
	EngineSystem::GetInst()->PostUpdate();

	SceneManager::GetInst()->PostUpdate(deltaTime);
}

int Engine::Run()
{
	MSG msg = { 0 };

	m_engineTimer.Reset();
	m_engineTimer.Start();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_engineTimer.Tick();
			static float totalTime = 0.0f;
			totalTime += m_engineTimer.DeltaTime();

			wstring text = L"Engine TotalTime: " + to_wstring(totalTime);
			SetWindowText(m_wndInfo.hWnd, text.c_str());

			Update(m_engineTimer.DeltaTime());
			Render(m_engineTimer.DeltaTime());
			PostUpdate(m_engineTimer.DeltaTime());
		}
	}

	return (int)msg.wParam;
}

float Engine::AspectRatio()
{
	return static_cast<float>(m_wndInfo.Width)/(m_wndInfo.Height);
}

BOOL Engine::InitWindowInstance(int nCmdShow)
{
	//	Convert to the client area
	RECT clientRect = { 0, 0, m_wndInfo.Width, m_wndInfo.Height };
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, false);
	m_wndInfo.Width	= clientRect.right - clientRect.left;
	m_wndInfo.Height	= clientRect.bottom - clientRect.top;

	//	Create Window Handle
	m_wndInfo.hWnd = CreateWindowW(L"DXEngine", L"DXEngine", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		m_wndInfo.Width, m_wndInfo.Height, nullptr, nullptr, m_hInstance, nullptr);

	if (!m_wndInfo.hWnd)
	{
		return FALSE;
	}

	ShowWindow(m_wndInfo.hWnd, m_wndInfo.WindowMode);
	UpdateWindow(m_wndInfo.hWnd);

	return TRUE;
}

ATOM Engine::MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = MainWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW + 1);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"DXEngine";
	wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

	return RegisterClassExW(&wcex);
}

LRESULT Engine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_paused = true;
			m_engineTimer.Stop();
		}
		else
		{
			m_paused = false;
			m_engineTimer.Start();
		}
		return 0;
	case WM_SIZE:
		m_wndInfo.Width = LOWORD(lParam);
		m_wndInfo.Height = HIWORD(lParam);
		return 0;
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_KEYUP:
		if (wParam == VK_ESCAPE)
		{
			PostQuitMessage(0);
		}
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:

		SceneManager::DestroyInst();

		ResourceManager::DestroyInst();

		EngineSystem::DestroyInst();

		EngineInput::DestroyInst();

		PostQuitMessage(0);

		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
