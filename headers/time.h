// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     time.h
//   Author:
//     Floris van Riel
//   Description:
//     Handles time for animations
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include <Windows.h>

#define WIN32_LEAN_AND_MEAN

// The timer class
class Time {
public:
	Time();
	~Time();

	// Constants for the time and interval in seconds
	float GameTime()const;
	float DeltaTime()const;

	// Methods for setting the time state
	void Reset();
	void Start();
	void Stop();
	void Tick();

private:
	// Private variables for interval
	double m_secondsPerCount;
	double m_deltaTime;

	__int64 m_baseTime;
	__int64 m_pausedTime;
	__int64 m_stopTime;
	__int64 m_prevTime;
	__int64 m_currTime;

	bool m_stopped;
};