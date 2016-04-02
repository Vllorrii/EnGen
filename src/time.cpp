// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     time.cpp
//   Author:
//     Floris van Riel
//   Description:
//     Provides methods to handle game time
//
// An AP product (c) 2016
// ------------------------------------ //

// Includes
#include "time.h"

Time::Time() {
	__int64 countsPerSec;

	m_secondsPerCount = 0.0;
	m_deltaTime = -1.0;
	m_baseTime = 0;
	m_pausedTime = 0;
	m_prevTime = 0;
	m_currTime = 0;
	m_stopped = false;

	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	m_secondsPerCount = 1.0 / (double)countsPerSec;
}

Time::~Time() {
}

void Time::Reset() {
	// Get the current time
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	// Reset the variables
	m_baseTime = currTime;
	m_prevTime = currTime;
	m_stopTime = 0;
	m_stopped = false;
}

void Time::Start() {
	__int64 startTime;
	
	// Get the current time
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	// If the time was not stopped do not continue
	if (!m_stopped) {
		return;
	}

	// Calculate the paused time
	m_pausedTime += startTime - m_stopTime;

	// The previous time is the start time
	m_prevTime = startTime;

	// Reset the variables
	m_stopTime = 0;
	m_stopped = false;
}

void Time::Stop() {
	// If the time is already stopped do not continue
	if (m_stopped) {
		return;
	}

	// Get the current time
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	// Save the time and set the state to stopped
	m_stopTime = currTime;
	m_stopped = true;
}

void Time::Tick() {
	__int64 currTime;

	// If the game is paused set interval to 0
	if (m_stopped) {
		m_deltaTime = 0.0;
		return;
	}

	// Get the current time
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	m_currTime = currTime;

	// Calculate the interval as the time difference
	m_deltaTime = (m_currTime - m_prevTime) * m_secondsPerCount;

	// Now set the current time as the previous time for the next frame
	m_prevTime = m_currTime;

	// Prevent the interval from getting negative
	if (m_deltaTime < 0.0) {
		m_deltaTime = 0.0;
	}

	return;
}

float Time::DeltaTime()const {
	return (float)m_deltaTime;
}

float Time::GameTime()const {
	// If the game is paused do not count the elapsed paused time
	// Else just use the current time
	if (m_stopped) {
		return (float)(((m_stopTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	} else {
		return (float)(((m_currTime - m_pausedTime) - m_baseTime) * m_secondsPerCount);
	}
}