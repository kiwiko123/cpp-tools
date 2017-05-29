#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdio.h>
#include "scheduler.hpp"
#include "algorithm_exception.hpp"


Scheduler::Scheduler(SchedulingAlgorithm* the_algorithm)
    : comparator{[](std::shared_ptr<Process> a, std::shared_ptr<Process> b){ return a->id() < b->id(); }},
      processes{comparator},
      algorithm{the_algorithm},
      current_job{nullptr}
{
}

Scheduler::~Scheduler()
{
    delete algorithm;
}

Scheduler::operator bool() const
{
    bool processes_empty = processes.empty();
    if (processes_empty && !ready_queue.empty())
    {
        throw AlgorithmException{"processes and ready_queue out of sync; both should be empty"};
    }
    return !processes_empty;
}

int Scheduler::operator++()
{
    load();
    if (!ready_queue.empty())
    {
        schedule();
    }
    return Process::increment_clock_time();
}

void Scheduler::read_process(int arrival_time, int total_time)
{
    int pid = processes.size();
    std::shared_ptr<Process> process{new Process{pid, arrival_time, total_time}};
    processes.insert(process);
}

void Scheduler::schedule()
{
    // get the next process to run from the scheduling algorithm,
    // and run it for 1 cycle.
    Process& p = sync_current_process();
    current_job->operator++();
    p = *current_job;

    if (current_job->finished())
    {
        remove_current_process();
    }

    // go through all other already-arrived processes and increment their waiting times
    for (auto& process: ready_queue)
    {
        if (*process != *current_job)
        {
            process->wait();
        }
    }
}

void Scheduler::start()
{
    while (*this)
    {
        operator++();
    }
    print_times();
    Process::reset_clock_time();
}

void Scheduler::print_times() const
{
    auto sum_total_times = [](int acc, const std::pair<int, int>& p){ return acc + p.second; };
    int sum = std::accumulate(stats.begin(), stats.end(), 0, sum_total_times);
    double average = sum / static_cast<double>(stats.size());

    std::string average_as_str = std::to_string(average);
    std::unique_ptr<char> allocated{new char[average_as_str.size() + 1]};
    char* buf = allocated.get();
    sprintf(buf, "%.2f", average);
    std::cout << buf;

    for (const auto& pair: stats)
    {
        std::cout << " " << pair.second;
    }
    std::cout << std::endl;
}

void Scheduler::load()
{
    for (auto& process: processes)
    {
        if (!process->finished() && process->arrival_time() == Process::clock_time())
        {
            ready_queue.insert(process);
            algorithm->add_process(*process);
        }
    }
}

void Scheduler::set_current_process(const Process& process_from_algorithm)
{
    auto locate = [&process_from_algorithm](std::shared_ptr<Process> p){ return *p == process_from_algorithm; };
    auto found = std::find_if(processes.begin(), processes.end(), locate);

    current_job = *found;
    if (current_job->arrived())
    {
        current_job->unblock();
    }
}

Process& Scheduler::sync_current_process()
{
    Process& p = algorithm->next_process();
    if (!current_job || *current_job != p)
    {
        set_current_process(p);
    }
    return p;
}

void Scheduler::remove_current_process()
{
    int pid = algorithm->pop_next_process();
    if (pid != current_job->id())
    {
        std::ostringstream buf;
        buf << "Scheduler::remove_current_process - " << algorithm->name() << " popped Process " << pid;
        buf << ", but expected Process " << current_job->id();
        throw AlgorithmException{buf.str()};
    }

    stats[pid] = current_job->total_time();
    ready_queue.erase(current_job);
    processes.erase(current_job);
}