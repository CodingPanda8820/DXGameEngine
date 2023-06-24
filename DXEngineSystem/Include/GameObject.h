#pragma once
#include "EngineSystem.h"
#include "EngineUtils.h"
#include "EngineMath.h"
#include "Transform.h"
#include "ResourceManager.h"

class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
	
public:
	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void PostUpdate();

public:
	void					SetShader(const string& shaderName);
	shared_ptr<Shader>		GetShader();

public:
	shared_ptr<Transform> GetTransform() { return m_transform; }

public:
	unique_ptr<UploadBuffer<CBGameObject>> m_attributes;

protected:
	virtual void UpdateAttributes();
	virtual void RenderAttributes();

protected:
	shared_ptr<Shader>		m_shader;
	shared_ptr<Transform>	m_transform;
};

