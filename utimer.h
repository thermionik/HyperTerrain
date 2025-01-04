// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////
#ifndef UNIXTIMER_H
#define UNIXTIMER_H

#include <sys/time.h>

#include "timer.h"

class UTimer : public Timer 
{
public:
	UTimer();
	virtual ~UTimer();
	double GetTime();
};

#endif
