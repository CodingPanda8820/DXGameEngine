#include "pch.h"
#include "EngineTimer.h"

EngineTimer::EngineTimer()
{
	m_secondsPerCount = 0.0f;
	m_totalTime = 0.0f;
	m_deltaTime = -1.0f;
	m_baseTime = 0;
	m_pausedTime = 0;
	m_previousTime = 0;
	m_currentTime = 0;
	m_stopped = false;

	__int64 countsPerSecond;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSecond);
	m_secondsPerCount = 1.0 / (double)countsPerSecond;
}

EngineTimer::~EngineTimer()
{

}

float EngineTimer::TotalTime() const
{
	if (m_stopped)
	{
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}

	else
	{
		return (float)(((m_currentTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
}

float EngineTimer::DeltaTime() const
{
	return (float)m_deltaTime;
}

void EngineTimer::Reset()
{
	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

	m_baseTime = currentTime;
	m_previousTime = currentTime;
	m_stopTime = 0;
	m_stopped = false;
}

void EngineTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (m_stopped)
	{
		m_pausedTime += (startTime - m_stopTime);

		m_previousTime = startTime;
		m_stopTime = 0;
		m_stopped = false;
	}
}

void EngineTimer::Stop()
{
	if (!m_stopped)
	{
		__int64 currentTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);

		m_stopTime = currentTime;
		m_stopped = true;
	}
}

void EngineTimer::Tick()
{
	if (m_stopped)
	{
		m_deltaTime = 0.0;
		return;
	}

	__int64 currentTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currentTime);
	m_currentTime = currentTime;

	m_deltaTime = (m_currentTime - m_previousTime) * m_secondsPerCount;
	m_totalTime += m_deltaTime;

	m_previousTime = m_currentTime;

	if (m_deltaTime < 0.0)
	{
		m_deltaTime = 0.0;
	}

}
