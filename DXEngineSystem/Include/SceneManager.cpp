#include "pch.h"
#include "SceneManager.h"

DEFINITION_SINGLE(SceneManager)

SceneManager::SceneManager()
{
	m_currentScene = make_shared<Scene>();
}

SceneManager::~SceneManager()
{

}

bool SceneManager::Init()
{
	m_currentScene->Init();

	return true;
}

void SceneManager::Update(const float& deltaTime)
{
	m_currentScene->Update(deltaTime);
}

void SceneManager::Render(const float& deltaTime)
{
	m_currentScene->Render(deltaTime);
}

void SceneManager::PostUpdate(const float& deltaTime)
{
	m_currentScene->PostUpdate(deltaTime);
}

void SceneManager::NewScene()
{
}
