#include <map>
#include <vector>
#include <cmath>
#include "mlf_algorithm.hpp"


MLFAlgorithm::MLFAlgorithm()
    : comparator{[this](const Process& a, const Process& b){ return priorities[a.id()].priority == priorities[b.id()].priority ?
                                                                    priorities[a.id()].entered > priorities[b.id()].entered :
                                                                    priorities[a.id()].priority > priorities[b.id()].priority; }},
      processes{comparator}
{
}

bool MLFAlgorithm::empty() const
{
    return processes.empty();
}

Process& MLFAlgorithm::get_next_process()
{
    Process& next = processes.top();
    if (next.finishes_next())
    {
        return next;
    }
    else
    {
        Process copy = next;
        int pid = copy.id();
        PriorityInfo& info = priorities[pid];

        info.quantum += 1;
        if (info.ready_for_preempt())
        {
            info.preempt();
        }

        processes.pop();
        processes.push(copy);
        return processes.get(copy);
    }
}

void MLFAlgorithm::add_process(const Process& process)
{
    priorities[process.id()] = PriorityInfo{0, 0, Process::clock_time()};
    processes.push(process);
}

int MLFAlgorithm::pop_next_process()
{
    int pid = processes.top().id();
    priorities.erase(pid);
    processes.pop();
    return pid;
}

std::string MLFAlgorithm::name() const
{
    return "MLF";
}

std::string MLFAlgorithm::to_string() const
{
    std::ostringstream buf;
    std::map<int, std::vector<int>> sorted;
    for (const auto& pair: priorities)
    {
        int pid = pair.first;
        const MLFAlgorithm::PriorityInfo& info = pair.second;
        sorted[info.priority].push_back(pid);
    }

    buf << "MLF(" << std::endl;
    for (const auto& pair: sorted)
    {
        buf << "  " << pair.first << ": [";
        for (const auto& pid: pair.second)
        {
            buf << pid << ", ";
        }
        buf << "]," << std::endl;
    }
    buf << ")";
    return buf.str();
}

bool MLFAlgorithm::PriorityInfo::ready_for_preempt() const
{
    return quantum >= std::pow(2, priority);
}

void MLFAlgorithm::PriorityInfo::preempt()
{
    quantum = 0;
    ++priority;
    entered = Process::clock_time();
}