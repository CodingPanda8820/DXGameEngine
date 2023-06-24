#include "ClientInfo.h"
#include "Engine.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdShow, int nCmdShow)
{
	if (!Engine::GetInst()->Init(hInstance, 1280, 720, true))
	{
		Engine::DestroyInst();
		return 0;
	}

	int Ret = Engine::GetInst()->Run();

	Engine::DestroyInst();

	return true;
}