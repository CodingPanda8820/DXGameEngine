#pragma once
#include "EngineUtils.h"
#include "Shader.h"
#include "Texture.h"
#include "GPUBufferUtils.h"

enum
{
	TEXTURE_PARM_COUNT = REGISTER_ID(TREGISTER_TYPE::COUNT),
};

class Material
{
public:
	Material();
	~Material();

public:
	void Update();
	void Render();

public:
	void SetName(const string& name);

	void SetDiffuse(const XMFLOAT4& rgba);
	void SetDiffuse(float r, float g, float b, float a);
	void SetSpecular(const XMFLOAT3& rgb);
	void SetSpecular(float r, float g, float b);
	void SetShininess(float value);
	void SetAmbient(const XMFLOAT4& rgba);
	void SetAmbient(float r, float g, float b, float a);

	void SetTexture(TREGISTER_TYPE type, shared_ptr<Texture> texture);
	void SetTexture(uint8 registerID, shared_ptr<Texture> texture);
	void SetUserDataInt(uint8 index, uint32 value);
	void SetUserDataFloat(uint8 index, float value);
	void SetUserDataMatrix(uint8 index, XMFLOAT4X4 matrix);

public:
	string		GetName();

	XMFLOAT4	GetDiffuse();
	XMFLOAT3	GetSpecular();
	float		GetShininess();
	XMFLOAT4	GetAmbient();

	uint32		GetUserDataInt(uint8 index);
	float		GetUserDataFloat(uint8 index);
	XMFLOAT4X4	GetUserDataMatrix(uint8 index);
	XMFLOAT4*	GetUserDataFloat4sStart();

public:
	unique_ptr<UploadBuffer<CBMaterial>> m_attributes;

	void RenderTextures();
	void UpdateAttributes();
	void RenderAttributes();

private:
	string		m_name;

	XMFLOAT4	m_diffuse;
	XMFLOAT3	m_specular;
	float		m_shininess;
	XMFLOAT4	m_ambient;

	shared_ptr<Shader>								m_shader;

	array<shared_ptr<Texture>, TEXTURE_PARM_COUNT>  m_textures;
	array<int32, TEXTURE_PARM_COUNT>				m_texturesOn;

	int32		m_userDataInt[MAX_USER_DATA_INT] = { 0 };
	float		m_userDataFloat[MAX_USER_DATA_FLOAT] = { 0.0f };
	XMFLOAT4X4	m_userDataMatrix[MAX_USER_DATA_MATRIX] = {};

	XMFLOAT4	m_userDataFloat4s[LENGHT_USER_DATA_FLOAT4S] = {};

private:
	ComPtr<ID3D12DescriptorHeap> m_descriptorHeap;
	uint32						 m_cpuHandleSize;
};

