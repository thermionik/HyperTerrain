// Timer.cpp: implementation of the Timer class.
//
//////////////////////////////////////////////////////////////////////

#include "utimer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UTimer::UTimer() : Timer()
{
//RateInverted = 1 / (double)1000000;
	RateInverted = 0.000001;
}

UTimer::~UTimer()
{

}

double UTimer::GetTime()
{
	timeval time;
	gettimeofday(&time, 0);
	
    return ((double)time.tv_sec + (double)time.tv_usec * RateInverted); 
}

