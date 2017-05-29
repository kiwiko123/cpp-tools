#ifndef PROJECT2_FIFO_ALGORITHM_HPP
#define PROJECT2_FIFO_ALGORITHM_HPP

#include <deque>
#include "scheduling_algorithm.hpp"


class FIFOAlgorithm : public SchedulingAlgorithm
{
public:
    // inherit SchedulingAlgorithm constructor
    using SchedulingAlgorithm::SchedulingAlgorithm;

    virtual bool empty() const override;
    virtual Process& get_next_process() override;
    virtual void add_process(const Process& process) override;
    virtual int pop_next_process() override;

    virtual std::string name() const override;
    virtual std::string to_string() const override;

private:
    std::deque<Process> processes;
};

#endif //PROJECT2_FIFO_ALGORITHM_HPP
