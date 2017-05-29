#ifndef PROJECT2_SRT_ALGORITHM_HPP
#define PROJECT2_SRT_ALGORITHM_HPP

#include <functional>
#include <queue>
#include <vector>
#include "scheduling_algorithm.hpp"


class SRTAlgorithm : public SchedulingAlgorithm
{
public:
    SRTAlgorithm();

    virtual bool empty() const override;
    virtual Process& get_next_process() override;
    virtual void add_process(const Process& process) override;
    virtual int pop_next_process() override;

    virtual std::string name() const override;
    virtual std::string to_string() const override;


private:
    typedef std::function<bool(const Process&, const Process&)> SRTComparator;
    typedef std::priority_queue<Process, std::vector<Process>, SRTComparator> PriorityQueue;
    SRTComparator comparator;
    PriorityQueue processes;
};

#endif //PROJECT2_SRT_ALGORITHM_HPP
