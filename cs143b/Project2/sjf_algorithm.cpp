#include "sjf_algorithm.hpp"


SJFAlgorithm::SJFAlgorithm()
    : comparator{[](const Process& a, const Process& b){ return a.run_time() == b.run_time() ?
                                                                a.id() > b.id() :
                                                                a.run_time() > b.run_time(); }},
      processes{comparator},
      current_process{nullptr}
{
}

bool SJFAlgorithm::empty() const
{
    return !current_process;
}

Process& SJFAlgorithm::get_next_process()
{
    if (!processes.empty())
    {
        // if a process is up next (but has not started yet),
        // and another process arrives AT THE SAME TIME with a SHORTER JOB TIME,
        // "swap" them (so that current_process becomes the newly arrived, shorter-time process,
        // and the original current_process is now up next at the top of the queue.
        const Process& next = processes.top();
        bool ready_to_start = current_process->remaining_time() == current_process->run_time();
        if (ready_to_start && current_process->run_time() > next.run_time())
        {
            Process copy{*current_process};
            current_process = std::shared_ptr<Process>{new Process{next}};
            processes.pop();
            processes.push(copy);
        }
    }

    return *current_process;
}

void SJFAlgorithm::add_process(const Process& process)
{
    if (empty())
    {
        current_process = std::shared_ptr<Process>{new Process{process}};
    }
    else
    {
        processes.push(process);
    }
}

int SJFAlgorithm::pop_next_process()
{
    if (empty())
    {
        throw AlgorithmException{"SJFAlgorithm::pop_next_process - empty"};
    }

    int pid = current_process->id();
    if (processes.empty())
    {
        current_process = nullptr;
    }
    else
    {
        current_process = std::shared_ptr<Process>{new Process{processes.top()}};
        processes.pop();
    }
    return pid;
}

std::string SJFAlgorithm::name() const
{
    return "SJF";
}

std::string SJFAlgorithm::to_string() const
{
    std::ostringstream buf;
    buf << name() << "(";
    if (!empty())
    {
        buf << *current_process;
        PriorityQueue copy{processes};
        while (!copy.empty())
        {
            buf << ", " << copy.top();
            copy.pop();
        }
    }
    buf << ")";
    return buf.str();
}