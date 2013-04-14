
#ifndef TIMER_H
#define TIMER_H

#include <iostream>
#include <unistd.h>
#include <sys/time.h>

class Timer
{

private:
	typedef timeval clock;
	clock start;

	// hold the elapsed time in milliseconds
	double elapsed;

	// keep track of the current status of the clock
	enum State
	{
		STOPPED,
		RUNNING
	} state;


	// Calculate the elapsed time of the clock	
	double GetElapsedTime( clock start, clock stop )
	{
		double sec  = stop.tv_sec - start.tv_sec;
		double usec = stop.tv_usec - start.tv_usec;
		return (sec * 1000 + usec / 1000.0);
	}

public:
	Timer() 
	{ 
		state = STOPPED; 
		elapsed = 0;

	}
	~Timer() {}

	/* starts (or restarts) the timer */
	void Start( void )
	{
		if( state == STOPPED )
		{
			memset(&start, 0 , sizeof( clock ));
			elapsed = 0;
			gettimeofday(&start, NULL);
			state = RUNNING;
		}
	}

	/* returns the elapsed time of the timer in milliseconds */
	double Elapsed ( void )
	{
		clock delta;
		gettimeofday( &delta, NULL );
		if (state == RUNNING)
		{
			return GetElapsedTime( start, delta );
			
		} else return elapsed;
	}

	void Restart ( void )
	{
		state = STOPPED;
		Start();
	}

	/* stops the current timer */
	void Stop ( void )
	{
		if ( state != STOPPED ) 
		{
			clock delta;
			gettimeofday( &delta, NULL );
			elapsed = GetElapsedTime( start, delta );
			state = STOPPED;			
		}
	}

};

#endif
