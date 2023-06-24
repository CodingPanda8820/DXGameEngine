#pragma once
#define _HAS_STD_BYTE 0

#include <Windows.h>
#include <windowsx.h>
#include <string>
#include <memory>
#include <algorithm>
#include <vector>
#include <array>
#include <deque>
#include <unordered_map>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <cassert>
using namespace std;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <filesystem>
namespace fileSystem = std::filesystem;

#include "d3dx12.h"
#include <dxgi1_4.h>
#include <d3d12.h>
#include <D3DCompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <DirectXTex/DirectXTex.h>
#include <DirectXTex/DirectXTex.inl>
using namespace DirectX;
using namespace DirectX::PackedVector;

#if defined(DEBUG) || defined(_DEBUG)
#pragma comment(lib, "DirectXTex\\DirectXTex_DEBUG.lib")
#else
#pragma comment(lib, "DirectXTex\\DirectXTex.lib")
#endif

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "FBX/fbxsdk.h"
#if defined (DEBUG) || defined(_DEBUG)
#pragma comment(lib, "FBX\\debug\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\debug\\libxml2-md.lib")
#pragma comment(lib, "FBX\\debug\\zlib-md.lib")
#else
#pragma comment(lib, "FBX\\release\\libfbxsdk-md.lib")
#pragma comment(lib, "FBX\\release\\libxml12-md.lib")
#pragma comment(lib, "FBX\\release\\zlib-md.lib")
#endif // _DEBUG

using int8	 = __int8;
using int16  = __int16;
using int32  = __int32;
using int64  = __int64;
using uint8	 = unsigned __int8;
using uint16 = unsigned __int16;
using uint32 = unsigned __int32;
using uint64 = unsigned __int64;

//	MACRO::Singleton
#define SAFE_DELETE(p) if(p) { delete p; p = nullptr;}

#define DECLARE_SINGLE(Type)\
private:\
	static Type* m_Inst;\
public:\
	static Type* GetInst()\
	{\
		if(!m_Inst)\
			m_Inst = new Type;\
		return m_Inst;\
	}\
	static void DestroyInst()\
	{\
		SAFE_DELETE(m_Inst);\
	}\
private:\
	Type();\
	~Type();\

#define DEFINITION_SINGLE(Type) Type* Type::m_Inst = nullptr;
#define GET_SINGLE(Type) Type::GetInst();

//	Macro Constant
#define BACK_BUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define DEPTH_STENCIL_FORMAT DXGI_FORMAT_D24_UNORM_S8_UINT
#define INDEX_BUFFER_FORMAT DXGI_FORMAT_R32_UINT

#define SWAP_CHAIN_BUFFER_COUNT 2
#define CUBE_MAP_BUFFER_COUNT 6

#define STATIC_ENV_SRV_PADDING	1
#define DYNAMIC_ENV_SRV_PADDING 2

enum class KEY_TYPE
{
	MLB = VK_LBUTTON,
	MRB = VK_RBUTTON,
	CANCLE = VK_CANCEL,
	MMB = VK_MBUTTON,
	XMB1 = VK_XBUTTON1,
	XMB2 = VK_XBUTTON2,
	BACK = VK_BACK,
	TAB = VK_TAB,
	CLEAR = VK_CLEAR,
	ENTER = VK_RETURN,
	SHIFT = VK_SHIFT,
	CTRL = VK_CONTROL,
	ALT = VK_MENU,
	PAUSE = VK_PAUSE,
	CAPS_LOCK = VK_CAPITAL,
	HANGUEL = VK_HANGUL,
	ESC = VK_ESCAPE,
	SPACEBAR = VK_SPACE,
	PAGE_UP = VK_PRIOR,
	PAGE_DOWN = VK_NEXT,
	END = VK_END,
	HOME = VK_HOME,
	LEFT = VK_LEFT,
	UP = VK_UP,
	RIGHT = VK_RIGHT,
	DOWN = VK_DOWN,
	SELECT = VK_SELECT,
	PRINT = VK_PRINT,
	EXECUTE = VK_EXECUTE,
	PRINT_SCREEN = VK_SNAPSHOT,
	INSERT = VK_INSERT,
	DEL = VK_DELETE,
	HELP = VK_HELP,
	KEY0 = 0x30, KEY1, KEY2, KEY3, KEY4, KEY5, KEY6, KEY7, KEY8, KEY9,
	A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	NUM0 = VK_NUMPAD0, NUM1, NUM2, NUM3, NUM4, NUM5, NUM6, NUM7, NUM8, NUM9,
	F1 = VK_F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	KEY_CLEAR = VK_OEM_CLEAR,
	KEY_TYPE_COUNT
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

//	Path Macro
#define PATH_SOLUTION	R"($(SolutionDir))"
#define PATH_ASSET		L"\\DXEngineResource\\Asset\\"