#ifndef TIME_H
#define TIME_H

#include <chrono>
#include <SDL.h>
#include <cstdint>
#include <iostream>

class Timer {
private:
	using clock = std::chrono::steady_clock;

	clock::time_point StartTime = {};
	clock::duration EnlapsedTime = {};

	float frameRateCap = 30;

public:
	void start();

	clock::duration getEnlapsed();
	float getEnlapsedFloat();
	float getEnlapsedFloatMS();

	void reset();

	void setFrameCap(float cap);
	float getFrameCap();

	void frameDelay();

};


extern Timer timer;


#endif