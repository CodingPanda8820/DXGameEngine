#pragma once
#include "EngineUtils.h"

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(KEY_TYPE::KEY_TYPE_COUNT),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class EngineInput
{
	DECLARE_SINGLE(EngineInput)

public:
	bool Init(WindowInfo wndInfo);
	void Update();

	POINT GetMousePosition() { return m_mousePos; }
	void SetMouseLastPosition(POINT position) { m_mousePos = position; }
	void SetMouseLastPosition(float x, float y) { m_mousePos.x = (LONG)x; m_mousePos.y = (LONG)y; }

	bool GetButton(KEY_TYPE btnState) { return GetState(btnState) == KEY_STATE::PRESS; }
	bool GetButtonDown(KEY_TYPE btnState) { return GetState(btnState) == KEY_STATE::DOWN; }
	bool GetButtonUp(KEY_TYPE btnState) { return GetState(btnState) == KEY_STATE::UP; }

private:
	inline KEY_STATE GetState(KEY_TYPE btnState) { return m_keyStates[static_cast<uint8>(btnState)]; }

private:
	HWND				m_hWnd;
	POINT				m_mousePos;
	vector<KEY_STATE>	m_keyStates;
};

