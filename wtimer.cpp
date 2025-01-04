// Timer.cpp: implementation of the Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "wtimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WTimer::WTimer()
{
	// Timer::Timer();
	LARGE_INTEGER ClockFreq;
	QueryPerformanceFrequency( &ClockFreq );
	RateInverted = 1 / (double)ClockFreq.QuadPart;
}

WTimer::~WTimer()
{

}

double WTimer::GetTime()
{
	LARGE_INTEGER Clock;

    QueryPerformanceCounter  ( &Clock ); 

    return (Clock.QuadPart) * RateInverted; 
}

