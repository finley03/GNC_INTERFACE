#ifndef TIME_H
#define TIME_H

#include "util.h"

// Class that acts as a configurable rate limiter
class Timer {
private:
	// just improves code readability
	using clock = std::chrono::steady_clock;

	clock::time_point startTime = {};
	clock::time_point pauseTime = {};

	bool paused = false;

	float rateCap = 30;

	// private function returns native C++ duration data type
	clock::duration getEnlapsedDuration();
public:
	// starts the timer at current time.
	void start();
	// initializes start time.
	void reset();
	// pause timer;
	void pause();
	// resume timer;
	void resume();

	// Returns enlapsed time since timer start.
	float getEnlapsed();
	// Returns enlapsed time since timer start
	// in milliseconds.
	float getEnlapsedMS();

	// Applies float rate cap
	void setRateCap(float cap);
	// Get the current rate cap. Returns
	// float in rate per second.
	float getRateCap();

	// This function will add the necessary delay when called
	// to pad the frame to the correct time.
	void delay();
};

#endif