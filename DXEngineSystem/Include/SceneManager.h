#pragma once
#include "Scene.h"

class SceneManager
{
	DECLARE_SINGLE(SceneManager)

public:
	bool Init();
	void Update(const float& deltaTime);
	void Render(const float& deltaTime);
	void PostUpdate(const float& deltaTime);

private:
	void NewScene();

private:
	shared_ptr<Scene> m_currentScene;

	unordered_map<string, shared_ptr<Scene>> m_scenes;
};

