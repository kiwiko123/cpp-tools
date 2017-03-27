#include "fifo_algorithm.hpp"



bool FIFOAlgorithm::empty() const
{
    return processes.empty();
}

Process& FIFOAlgorithm::get_next_process()
{
    return processes.front();
}

void FIFOAlgorithm::add_process(const Process& process)
{
    processes.push_back(process);
}

int FIFOAlgorithm::pop_next_process()
{
    int pid = processes.front().id();
    processes.pop_front();
    return pid;
}

std::string FIFOAlgorithm::name() const
{
    return "FIFO";
}

std::string FIFOAlgorithm::to_string() const
{
    std::ostringstream buf;
    buf << name() << "(";
    if (!empty())
    {
        auto i = processes.begin();
        buf << *(i++);
        for (; i != processes.end(); ++i)
        {
            buf << ", " << *i;
        }
    }
    buf << ")";
    return buf.str();
}