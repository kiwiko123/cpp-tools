#ifndef PROJECT2_SCHEDULING_ALGORITHM_HPP
#define PROJECT2_SCHEDULING_ALGORITHM_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "process.hpp"
#include "algorithm_exception.hpp"


class SchedulingAlgorithm
{
public:
    SchedulingAlgorithm();
    virtual ~SchedulingAlgorithm();

    friend std::ostream& operator<<(std::ostream& os, const SchedulingAlgorithm& alg);

    /* Returns a reference to the next process to be run.
     */
    Process& next_process();

    /* Adds a process using this algorithm.
     */
    virtual void add_process(const Process& process) = 0;

    /* Removes the next process from the algorithm.
     * Returns the pid of the process that was removed.
     *   FIFO: std::deque::pop_front
     *   SJF:  std::priority_queue::pop
     *   SRT:  std::priority_queue::pop
     *   MLF:  MLFQueue::pop
     */
    virtual int pop_next_process() = 0;

    /* Returns true if there all processes have completed,
     * or false if not.
     */
    virtual bool empty() const = 0;

    virtual std::string name() const = 0;

    virtual std::string to_string() const = 0;


protected:
    virtual Process& get_next_process() = 0;
};

#endif //PROJECT2_SCHEDULING_ALGORITHM_HPP
