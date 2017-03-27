#include "scheduling_algorithm.hpp"


SchedulingAlgorithm::SchedulingAlgorithm()
{
}

SchedulingAlgorithm::~SchedulingAlgorithm()
{
}

std::ostream& operator<<(std::ostream& os, const SchedulingAlgorithm& alg)
{
    os << alg.to_string();
    return os;
}

Process& SchedulingAlgorithm::next_process()
{
    Process& next = get_next_process();
    if (next.arrived())
    {
        next.unblock();
    }
    return next;
}