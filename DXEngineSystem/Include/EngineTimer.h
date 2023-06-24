#ifndef ENGINETIMER_H
#define ENGINETIMER_H

class EngineTimer
{
public:
	EngineTimer();
	~EngineTimer();

public:
	float TotalTime() const;
	float DeltaTime() const;

	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	double m_secondsPerCount;
	double m_deltaTime;
	double m_totalTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_previousTime;
	__int64 m_currentTime;

	bool m_stopped;
};

#endif