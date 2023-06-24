#include "pch.h"
#include "EngineInput.h"

DEFINITION_SINGLE(EngineInput)

EngineInput::EngineInput()
{

}

EngineInput::~EngineInput()
{

}

bool EngineInput::Init(WindowInfo wndInfo)
{
	m_hWnd = wndInfo.hWnd;
	m_keyStates.resize(KEY_TYPE_COUNT, KEY_STATE::NONE);

	return true;
}

void EngineInput::Update()
{
	HWND hWnd = GetActiveWindow();
	GetCursorPos(&m_mousePos);

	if (m_hWnd != hWnd)
	{
		for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
			m_keyStates[key] = KEY_STATE::NONE;

		return;
	}

	for (uint32 key = 0; key < KEY_TYPE_COUNT; key++)
	{
		KEY_STATE& state = m_keyStates[key];

		if (GetAsyncKeyState(key) & 0x8000)
		{
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::PRESS;
			else
				state = KEY_STATE::DOWN;
		}

		else
		{
			if (state == KEY_STATE::PRESS || state == KEY_STATE::DOWN)
				state = KEY_STATE::UP;
			else
				state = KEY_STATE::NONE;
		}
	}
}
