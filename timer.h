// Timer.h: interface for the Timer class.
//
//////////////////////////////////////////////////////////////////////
#ifndef TIMER_H
#define TIMER_H

typedef struct timet{
	int hour;
	int min;
	int sec;
	int msec;
} Time;

class Timer  
{
	protected:
		Timer();
	public:
		virtual ~Timer();
		virtual	double GetTime() = 0;
		void Start();
		void Stop();
		double GetElapsed();
		Time GetElapsedTime();

	protected:
		double LastTime;
		double TotalTime;
		double RateInverted;
};

#endif
