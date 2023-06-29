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
	virtual void Update() override;
	virtual void Render(OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);
	virtual void PostUpdate();

private:
	void RenderObject();
	void RenderShadow();

public:
	void SetName(const string& name);
	void SetLightType(LIGHT_TYPE type);
	void SetLightIndex(uint32 index);

	void SetStrength(XMFLOAT3& value);
	void SetStrength(float x, float y, float z);
	void SetFalloffStart(float value);
	void SetFalloffEnd(float value);
	void SetSpotPower(float value);

public:
	string					GetName();
	LIGHT_TYPE				GetLightType();
	uint32					GetLightIndex();
	XMFLOAT3				GetStrength();
	XMFLOAT3				GetPosition();
	XMFLOAT3				GetDirection();

	float					GetSpotPower();
	float					GetFalloffStart();
	float					GetFalloffEnd();

	shared_ptr<Material>	GetMaterial();
	shared_ptr<LightShape>	GetLightShape();
	shared_ptr<PolySurface> GetVolumeShape();

private:
	void UpdateLightMaterial(OBJECT_RENDER_TYPE type = OBJECT_RENDER_TYPE::OBJECT);

	void UpdateAttributes();
	void RenderAttributes();

private:
	unique_ptr<UploadBuffer<CBLight>>  m_attributes;

private:
	string								m_name;

	LIGHT_TYPE							m_lightType;
	uint32								m_lightIndex;
	shared_ptr<LightShape>				m_lightShape;
	shared_ptr<PolySurface>				m_lightPoint;
};

