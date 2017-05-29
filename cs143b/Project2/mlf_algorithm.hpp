#ifndef PROJECT2_MLF_ALGORITHM_HPP
#define PROJECT2_MLF_ALGORITHM_HPP

#include <functional>
#include <iostream>
#include <unordered_map>
#include "scheduling_algorithm.hpp"
#include "mlf_queue.hpp"



class MLFAlgorithm : public SchedulingAlgorithm
{
public:
    MLFAlgorithm();

    virtual bool empty() const override;
    virtual Process& get_next_process() override;
    virtual void add_process(const Process& process) override;
    virtual int pop_next_process() override;

    virtual std::string name() const override;
    virtual std::string to_string() const override;


private:
    struct PriorityInfo
    {
        int priority;
        int quantum;    // time spent so far in the current priority level
        int entered;    // time entered the current priority level

        bool ready_for_preempt() const;
        void preempt();
    };

    typedef std::function<bool(const Process&, const Process&)> MLFComparator;
    typedef MLFQueue<Process> PriorityQueue;

    std::unordered_map<int, PriorityInfo> priorities;    // {pid: PriorityInfo}
    MLFComparator comparator;
    PriorityQueue processes;
};

#endif //PROJECT2_MLF_ALGORITHM_HPP
