// Timer.cpp: implementation of the Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "timer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Timer::Timer()
{
	TotalTime = 0;
	LastTime = 0;
	RateInverted = 0;
}

Timer::~Timer()
{

}

void Timer::Start() 
{ 
	LastTime = GetTime(); 
	TotalTime = 0.0;
}

void Timer::Stop()
{
	double temp = LastTime;
	LastTime = GetTime();
    TotalTime += LastTime - temp; 
}

double Timer::GetElapsed()
{
	return TotalTime;
}

Time Timer::GetElapsedTime()
{	
	Time time;
	
	double elapsed = GetElapsed();
	double temp = elapsed - ((int)elapsed / 1);
	
	time.msec = (int)temp * 1000;
	time.sec = (int)elapsed % 60;
	time.min = (int)elapsed / 60;
	time.hour = time.min / 60;
	
	if ( time.hour )
		time.min -= time.hour * 60;

	return time;
};
