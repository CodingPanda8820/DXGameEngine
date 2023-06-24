#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DXEngineSystem_DEBUG.lib")
#else
#pragma comment(lib, "DXEngineSystem.lib")
#endif

#include <EnginePch.h>