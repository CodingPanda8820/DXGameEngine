#pragma once
#include "EngineMath.h"

UINT	ConvertToMinimumHardwareAllocationByteSize(UINT byteSize);

wstring ConvertStringToWString(const string& value);
string  ConvertWStringToString(const wstring& value);

struct WindowInfo
{
	HWND	hWnd = nullptr;
	float	Width = 0.0f;
	float	Height = 0.0f;
	bool	WindowMode = true;
};

struct MultiSampleAntiAliasing
{
	bool State = false;
	UINT Quality = 0;
};

struct CBGameObject
{
	XMFLOAT4X4 World = EngineMath::Identity4x4();
};

#define MAX_USER_DATA_INT			4
#define MAX_USER_DATA_FLOAT		4
#define MAX_USER_DATA_MATRIX	4
struct CBMaterial
{
	XMFLOAT4 Diffuse = { 1.0f, 1.0f, 1.0f, 1.0f };
	XMFLOAT3 Specular = { 0.01f, 0.01f, 0.01f };
	float	 Shininess = 1.0f;
	XMFLOAT4 Ambient = { 0.0f, 0.0f, 0.0f, 0.0f };

	int32	 DiffuseTexture2D = 0;
	int32	 SpecularTexture2D = 0;
	int32	 ShininessTexture2D = 0;
	int32	 NormalTexture2D = 0;

	int32	UserDataInt_0 = 0;
	int32	UserDataInt_1 = 0;
	int32	UserDataInt_2 = 0;
	int32	UserDataInt_3 = 0;

	float	UserDataFloat_0 = 0;
	float	UserDataFloat_1 = 0;
	float	UserDataFloat_2 = 0;
	float	UserDataFloat_3 = 0;

	XMFLOAT4X4 UserDataMatrix_0 = EngineMath::Identity4x4();
	XMFLOAT4X4 UserDataMatrix_1 = EngineMath::Identity4x4();
	XMFLOAT4X4 UserDataMatrix_2 = EngineMath::Identity4x4();
	XMFLOAT4X4 UserDataMatrix_3 = EngineMath::Identity4x4();
};

struct CBLight
{
	XMFLOAT3	Strength = { 0.5f, 0.5f, 0.5f };
	float		FalloffStart = 1.0f;
	XMFLOAT3	Direction = { 0.0f, -1.0f, 0.0f };
	float		FalloffEnd = 10.0f;
	XMFLOAT3	Position = { 0.0f, 0.0f, 0.0f };
	float		SpotPower = 64.0f;
};

#define MAX_LIGHT_COUNT 64
struct CBLightGroup
{
	CBLight		 Light[MAX_LIGHT_COUNT];
};

struct CBCamera
{
	XMFLOAT4X4 View = EngineMath::Identity4x4();
	XMFLOAT4X4 ViewInv = EngineMath::Identity4x4();
	XMFLOAT4X4 Projection = EngineMath::Identity4x4();
	XMFLOAT4X4 ProjectionInv = EngineMath::Identity4x4();
	XMFLOAT4X4 ViewProjection = EngineMath::Identity4x4();
	XMFLOAT4X4 ViewProjectionInv = EngineMath::Identity4x4();

	XMFLOAT3 EyePosition = { 0.0f, 0.0f, 0.0f };
	float	 EyePositionPad = 0.0f;

	XMFLOAT4	FogColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	float		FogStart = 100.0f;
	float		FogEnd = 5000.0f;
	float		FogPad0 = 0.0f;
	float		FogPad1 = 0.0f;
};

struct CBScene
{
	float TotalTime;
	float DeltaTime;
};

struct Point
{
	float X;
	float Y;
	float Z;
};

struct Vertex
{
	Vertex() {}
	Vertex(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT3& tangentu, const XMFLOAT2& uv)
	{
		Position = position;
		Normal = normal;
		TangentU = tangentu;
		UV = uv;
	}
	Vertex(float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v)
	{
		Position = XMFLOAT3(px, py, pz);
		Normal = XMFLOAT3(nx, ny, nz);
		TangentU = XMFLOAT3(tx, ty, tz);
		UV = XMFLOAT2(u, v);
	}
	bool operator == (const Vertex& arg) const
	{
		if (Position.x != arg.Position.x || Position.y != arg.Position.y || Position.z != arg.Position.z)
			return false;
		if (Normal.x != arg.Normal.x || Normal.y != arg.Normal.y || Normal.z != arg.Normal.z)
			return false;
		if (TangentU.x != arg.TangentU.x || TangentU.y != arg.TangentU.y || TangentU.z != arg.TangentU.z)
			return false;
		if (UV.x != arg.UV.x || UV.y != arg.UV.y)
			return false;

		return true;
	}

	XMFLOAT3 Position = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 Normal = { 0.0f, 1.0f, 0.0f };
	XMFLOAT3 TangentU = { 1.0f, 0.0f, 0.0f };
	XMFLOAT2 UV = { 0.0f, 0.0f };
};

struct PolySurfaceShape
{
	uint32 IndexCount = 0;
	uint32 StartIndexLocation = 0;
	uint32 StartVertexLocation = 0;

	string MaterialName = "";
};

struct PolySurfaceInfo
{
	string name;

	vector<Vertex>	Vertices;
	vector<uint32>	Indices;

	vector<PolySurfaceShape> PolySurfaceShapes;
};

struct SurfaceMaterialElementInfo
{
	XMFLOAT4	Diffuse;
	XMFLOAT4	Specular;
	XMFLOAT4	Ambient;
	XMFLOAT4	Shininess;

	wstring		DiffuseTexturePath;
	wstring		SpecularTexturePath;
	wstring		AmbientTexturePath;
	wstring		ShininessTexturePath;
	wstring		NormalTexturePath;

	string		Name;
};

enum class PROJECTION_TYPE
{
	PERSPECTIVE = 0,
	ORTHOGONAL,
};

enum class LIGHT_TYPE : uint8
{
	AMBIENT,
	DIRECTIONAL,
	POINT,
	SPOT,
	COUNT,
};

struct AssetInfo
{
	vector<string>	Materials;
	vector<string>	PolySurfaces;
};


enum class CREGISTER_TYPE : uint8
{
	GEOMETRY,
	MATERIAL,
	LIGHT,
	CAMERA,
	SCENE,
	SRV_DESCRIPTOR_TABLE,
	COUNT
};

enum class TREGISTER_TYPE : uint8
{
	DIFFUSE_TEXTURE2D,
	SPECULAR_TEXTURE2D,
	SHININESS_TEXTURE2D,
	NORMAL_TEXTURE2D,
	COUNT,
};

enum class GAMEOBJECT_TYPE : uint8
{
	DEFAULT = 0,
	CAMERA,
	GEOMETRY,
	LIGHT,
	COUNT
};

enum class COMPONENT_TYPE : uint8
{
	TRANSFORM,
	SHADER,
	MATERIAL,
	POLYSURFACE,
	TEXTURE,
	LIGHTSHAPE,
	CAMERASHAPE,
	PARTICLE,
	COUNT,
};

enum class RENDER_LAYER : uint8
{
	UI,
	SKYBOX,
	SRGB,
	COUNT
};

#define REGISTER_ID(X)		static_cast<uint8>(X)
#define RENDER_LAYER_ID(X)	static_cast<uint8>(X)