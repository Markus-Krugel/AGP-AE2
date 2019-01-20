#include "GameTimer.h"

GameTimer::GameTimer()
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	mSecondsPerCount = 1 / (double)countsPerSec;

	mDeltaTime = -1;

	mBaseTime = 0;
	mPausedTime = 0;
	mStopTime = 0;
	mPrevTime = 0;
	mCurrentTime = 0;

	mStopped = false;
}

float GameTimer::GameTime() const
{
	if (mStopped)
		return (float)(((mStopTime - mPausedTime) - mBaseTime) *mSecondsPerCount);
	else
		return (float)(((mCurrentTime - mPausedTime) - mBaseTime) *mSecondsPerCount);
}

float GameTimer::DeltaTime() const
{
	return(float)mDeltaTime;
}

void GameTimer::Reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	mBaseTime = currTime;
	mPrevTime = currTime;
	mStopTime = 0;
	mStopped = false;
}

void GameTimer::Start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);

	if (mStopped)
	{
		mPrevTime = startTime;
		mStopTime = 0;
		mStopped = false;
	}
}

void GameTimer::Stop()
{
	if (!mStopped)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&mStopTime);
		mStopped = true;
	}
}

void GameTimer::Tick()
{
	if (mStopped)
	{
		mDeltaTime = 0;
		return;
	}

	QueryPerformanceCounter((LARGE_INTEGER*)&mCurrentTime);

	mDeltaTime = (mCurrentTime - mPrevTime) *mSecondsPerCount;
	mPrevTime = mCurrentTime;

	if (mDeltaTime < 0)
		mDeltaTime = 0;
}
