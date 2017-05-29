#ifndef PROJECT2_SJFALGORITHM_HPP
#define PROJECT2_SJFALGORITHM_HPP

#include <functional>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>
#include "scheduling_algorithm.hpp"


class SJFAlgorithm : public SchedulingAlgorithm
{
public:
    SJFAlgorithm();

    virtual bool empty() const override;
    virtual Process& get_next_process() override;
    virtual void add_process(const Process& process) override;
    virtual int pop_next_process() override;

    virtual std::string name() const override;
    virtual std::string to_string() const override;


protected:
    typedef std::function<bool(const Process&, const Process&)> SJFComparator;
    typedef std::priority_queue<Process, std::vector<Process>, SJFComparator> PriorityQueue;
    SJFComparator comparator;
    PriorityQueue processes;


private:
    std::shared_ptr<Process> current_process;
};

#endif //PROJECT2_SJFALGORITHM_HPP
