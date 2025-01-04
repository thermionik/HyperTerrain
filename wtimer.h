// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////

#ifndef WINDOWSTIMER_H
#define WINDOWSTIMER_H

#include <windows.h>            
#include <mmsystem.h>

#include "timer.h"

class WTimer : public Timer 
{
public:
	WTimer();
	~WTimer();
	double GetTime();
};

#endif
