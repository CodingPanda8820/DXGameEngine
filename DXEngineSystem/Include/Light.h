#pragma once
#include "GameObject.h"
#include "LightShape.h"

class Light : public GameObject
{
public:
	Light();
	virtual ~Light();

public:
	virtual void Init(const string& name, LIGHT_TYPE type = LIGHT_TYPE::AMBIENT);
	virtual void Update();
	virtual void Render();
	virtual void PostUpdate();

public:
	void SetName(const string& name);
	void SetType(LIGHT_TYPE type);
	void SetMaterial(const string& name);
	void SetVolumeShape(const string& name);

	void SetStrength(XMFLOAT3& value);
	void SetStrength(float x, float y, float z);
	void SetFalloffStart(float value); 
	void SetFalloffEnd(float value); 
	void SetSpotPower(float value); 

	void SetPosition(XMFLOAT3& value); 
	void SetPosition(float x, float y, float z); 
	void SetDirection(XMFLOAT3& value); 
	void SetDirection(float x, float y, float z); 

public:
	string		GetName();
	LIGHT_TYPE	GetType();

	XMFLOAT3	GetStrength();
	XMFLOAT3	GetPosition();
	XMFLOAT3	GetDirection();

	float		GetSpotPower();
	float		GetFalloffStart();
	float		GetFalloffEnd();

	shared_ptr<Material>	GetMaterial();
	shared_ptr<LightShape>	GetLightShape();
	shared_ptr<PolySurface> GetVolumeShape();

private:
	virtual void UpdateAttributes() override;
	virtual void RenderAttributes() override;

private:
	unique_ptr<UploadBuffer<CBLight>>  m_attributes;

private:
	string								m_name;
	LIGHT_TYPE							m_type;
	shared_ptr<Material>				m_material;
	shared_ptr<LightShape>				m_lightShape;
	shared_ptr<PolySurface>				m_volumeShape;
};

