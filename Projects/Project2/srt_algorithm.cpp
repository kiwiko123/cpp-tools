#include "srt_algorithm.hpp"


SRTAlgorithm::SRTAlgorithm()
    : comparator{[](const Process& a, const Process& b){ return a.remaining_time() == b.remaining_time() ?
                                                                  a.id() > b.id() :
                                                                  a.remaining_time() > b.remaining_time(); }},
      processes{comparator}
{
}

bool SRTAlgorithm::empty() const
{
    return processes.empty();
}

Process& SRTAlgorithm::get_next_process()
{
    // std::priority_queue::top returns const reference,
    // SchedulingAlgorithm::get_next_process() returns a non-const reference
    return const_cast<Process&>(processes.top());
}

void SRTAlgorithm::add_process(const Process& process)
{
    processes.push(process);
}

int SRTAlgorithm::pop_next_process()
{
    int pid = processes.top().id();
    processes.pop();
    return pid;
}

std::string SRTAlgorithm::name() const
{
    return "SRT";
}

std::string SRTAlgorithm::to_string() const
{
    std::ostringstream buf;
    buf << name() << "(";
    PriorityQueue copy{processes};
    if (!copy.empty())
    {
        buf << copy.top();
        copy.pop();

        while (!copy.empty())
        {
            buf << ", " << copy.top();
            copy.pop();
        }
    }
    buf << ")";
    return buf.str();
}