#include "pch.h"
#include "GameTimer.h"


CGameTimer* CGameTimer::GetSharedTimer()
{
	// TODO: insert return statement here
	static CGameTimer* pTimer = nullptr;

	if (pTimer == nullptr)
	{
		pTimer = new CGameTimer();
	}

	return pTimer;
}

float CGameTimer::GetTotalTime() const
{
	if (m_bStopped)
	{
		return (m_stopTime - m_baseTime - m_pausedDuration) * m_durationPerCount;
	}
	else
	{
		return (m_currentTime - m_baseTime - m_pausedDuration) * m_durationPerCount;
	}
}

void CGameTimer::Tick()
{
	if (m_bStopped)
	{
		m_deltaTime = 0;
		return;
	}
	else
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&m_currentTime);

		m_deltaTime = (m_currentTime - m_prevTime) * m_durationPerCount;
		m_prevTime = m_currentTime;

		if (m_deltaTime < 0)
		{
			m_deltaTime = 0;
		}
	}
}

void CGameTimer::Pause()
{
	assert(m_bStopped == false);
	if (m_bStopped == false)
	{
		__int64 time;
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
		
		m_stopTime = time;
		m_bStopped = true;
	}
}

void CGameTimer::Resume()
{
	assert(m_bStopped == true);
	if (m_bStopped == true)
	{
		__int64 time;
		QueryPerformanceCounter((LARGE_INTEGER*)&time);

		m_pausedDuration += time - m_stopTime;
		m_stopTime = 0;
		m_bStopped = false;
		m_prevTime = time;
	}
}

void CGameTimer::Reset()
{
	__int64 time;
	QueryPerformanceCounter((LARGE_INTEGER*)&time);
	
	m_baseTime = time;
	m_prevTime = time;
	m_bStopped = false;
	m_stopTime = 0;
}

float CGameTimer::GetDeltaTime() const
{
	return m_deltaTime;
}



CGameTimer::CGameTimer()
{
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	m_durationPerCount = 1 / (float)freq.QuadPart;
}

