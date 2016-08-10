// This header defines a simple timer class that records in milliseconds.
//
// Author: Geoffrey Ko (2016)
#ifndef MS_TIMER_HPP
#define MS_TIMER_HPP

#include <chrono>
#include <ostream>

class ms_timer
{
public:
	ms_timer(bool active = false, bool fwd = true);

	friend std::ostream& operator<<(std::ostream& os, ms_timer& t);

	void start(bool fwd = true);
	void stop();
	double read();
	void reset();	// stops the clock if running
	void restart(bool fwd = true);


private:
	typedef std::chrono::high_resolution_clock HR_CLOCK_T;

	bool on;
	bool forward;
	HR_CLOCK_T::time_point started, ended;
};

#endif // MS_TIMER_HPP
