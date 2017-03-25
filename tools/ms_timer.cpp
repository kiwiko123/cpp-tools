#include <chrono>
#include <iostream>
#include "ms_timer.hpp"


ms_timer::ms_timer(bool active, bool fwd)
	: on{active}, forward{fwd}
{
	if (on)
		start();
}

std::ostream& operator<<(std::ostream& os, ms_timer& t)
{
	os << "ms_timer(" << t.read() << ")";
	return os;
}

void ms_timer::start(bool fwd)
{
	started = HR_CLOCK_T::now();
	on = true;
	forward = fwd;
}

void ms_timer::stop()
{
	ended = HR_CLOCK_T::now();
	on = false;
}

double ms_timer::read()
{
	if (on)
		ended = HR_CLOCK_T::now();

	return std::chrono::duration<double, std::milli>(forward ? ended - started : started - ended).count();
}

void ms_timer::reset()
{
	on = false;
	started = ended = HR_CLOCK_T::now();
}

void ms_timer::restart(bool fwd)
{
	reset();
	start(fwd);
}
