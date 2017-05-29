#include <cmath>
#include "process.hpp"


int Process::c_time = 0;

Process::Process(int process_id, int ar_time, int r_time)
    : pid{process_id}, arrival{ar_time}, run{r_time}, waiting{0}, current{0},
      done{false}, blocked{true}
{
}

std::ostream& operator<<(std::ostream& os, const Process& p)
{
    os << "[id=" << p.id() << ", ";
    os << "c=" << p.current_time() << ", ";
    os << "r=" << p.run_time() << ", ";
    os << "re=" << p.remaining_time() << ", ";
    os << "w=" << p.waiting_time();
    os << "]";

    return os;
}

bool Process::operator==(const Process& other) const
{
    return this == &other || id() == other.id();
}

bool Process::operator!=(const Process& other) const
{
    return !operator==(other);
}

int Process::operator++()
{
    if (blocked)
    {
        wait();
        return -1;
    }

    ++current;
    if (remaining_time() == 0)
    {
        done = true;
    }
    return current;
}

int Process::id() const
{
    return pid;
}

int Process::arrival_time() const
{
    return arrival;
}

int Process::run_time() const
{
    return run;
}

int Process::total_time() const
{
    return run_time() + waiting_time();
}

int Process::waiting_time() const
{
    return waiting;
}

int Process::current_time() const
{
    return current;
}

// static
int Process::clock_time()
{
    return c_time;
}

int Process::remaining_time() const
{
    return run_time() - current_time();
}

bool Process::arrived() const
{
    return arrival_time() <= Process::clock_time();
}

bool Process::finished() const
{
    return done;
}

bool Process::finishes_next() const
{
    return remaining_time() == 1;
}

bool Process::is_blocked() const
{
    return blocked;
}

void Process::block()
{
    blocked = true;
}

void Process::unblock()
{
    blocked = false;
}

void Process::wait()
{
    ++waiting;
}

int Process::increment_clock_time()
{
    return ++c_time;
}

void Process::reset_clock_time()
{
    c_time = 0;
}