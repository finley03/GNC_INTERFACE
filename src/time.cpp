#include "time.h"


Timer timer;


void Timer::start() {
	StartTime = clock::now();
}


void Timer::reset() {
	StartTime = {};
	EnlapsedTime = {};
}


Timer::clock::duration Timer::getEnlapsed() {
	EnlapsedTime = clock::now() - StartTime;
	return EnlapsedTime;
}

float Timer::getEnlapsedFloat() {
	return std::chrono::duration<float>(getEnlapsed()).count();
}

float Timer::getEnlapsedFloatMS() {
	return std::chrono::duration<float>(getEnlapsed()).count() * 1000;
}


void Timer::setFrameCap(float cap) {
	frameRateCap = cap;
}

float Timer::getFrameCap() {
	return frameRateCap;
}

void Timer::frameDelay() {
	float renderTime = getEnlapsedFloatMS();
	if (renderTime < 1000.0f / frameRateCap) {
		SDL_Delay((uint32_t)((1000.0f / frameRateCap) - renderTime));
	}
	reset();
	start();
}