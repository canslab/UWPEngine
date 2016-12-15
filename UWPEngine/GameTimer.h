#pragma once
class CGameTimer
{
public:
	static CGameTimer* GetSharedTimer();

	// Get (current - base) duration
	float GetTotalTime() const;

	// Record Time Stamp! 
	void Tick();
	void Pause();

	// Transition Stop mode -> Running Mode
	void Resume();
	void Reset();
	float GetDeltaTime() const;
	
private:
	CGameTimer();

	float m_durationPerCount;

	float m_deltaTime;
	float m_pausedDuration;
	
	__int64 m_stopTime;
	__int64 m_baseTime;
	__int64 m_currentTime;
	__int64 m_prevTime;

	bool m_bStopped;
};

