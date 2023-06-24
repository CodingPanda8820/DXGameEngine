#pragma once
#include "EngineSystem.h"
#include "EngineUtils.h"
#include "EngineMath.h"
#include "Transform.h"
#include "ResourceManager.h"

enum class OBJECT_RENDER_TYPE : uint8
{
	OBJECT,
	SHADOW,
	COUNT
};

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();

public:
	virtual void Init();
	virtual void Update();
	virtual void Render(OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);
	virtual void PostUpdate();

protected:
	void RenderObject();
	void RenderShadow();

public:
	void SetShader(const string& shaderName, OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);
	void SetMaterial(const string& materialName, OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);

	void SetScale(const XMFLOAT3& scale);
	void SetScale(const XMVECTOR& scale);
	void SetScale(float sx, float sy, float sz);
	void SetRotate(const XMFLOAT3& rotate);
	void SetRotate(const XMVECTOR& rotate);
	void SetRotate(float rx, float ry, float rz);
	void SetTranslate(const XMFLOAT3& translate);
	void SetTranslate(const XMVECTOR& translate);
	void SetTranslate(float tx, float ty, float tz);

public:
	shared_ptr<Shader>		GetShader();
	shared_ptr<Transform>	GetTransform();

public:
	unique_ptr<UploadBuffer<CBGameObject>> m_attributes;

protected:
	void UpdateAttributes();
	void RenderAttributes();

protected:
	shared_ptr<Shader>		m_shader;
	shared_ptr<Shader>		m_shader_shadow;

	shared_ptr<Material>	m_material;
	shared_ptr<Material>	m_material_shadow;

	shared_ptr<Transform>	m_transform;
};

