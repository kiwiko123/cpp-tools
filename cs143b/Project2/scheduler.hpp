#ifndef PROJECT2_SCHEDULER_HPP
#define PROJECT2_SCHEDULER_HPP

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <unordered_set>
#include "scheduling_algorithm.hpp"
#include "process.hpp"


class Scheduler
{
public:
    /* Takes in a raw pointer to a SchedulingAlgorithm derivative (for polymorphic purposes).
     * Assumes that the_algorithm is dynamically allocated,
     * as the destructor will delete it.
     */
    explicit Scheduler(SchedulingAlgorithm* the_algorithm);
    ~Scheduler();

    /* Returns true if there are still processes to be scheduled,
     * or false if not (i.e., all have finished).
     */
    operator bool() const;

    /* Advances the scheduler by 1 "cycle".
     * Returns the updated current time (after the cycle).
     */
    int operator++();

    /* Creates a new process with the corresponding arguments.
     * Adds it to the stored processes.
     */
    void read_process(int arrival_time, int total_time);

    /* Advances the state of the currently running process.
     * Updates statistics of waiting processes.
     */
    void schedule();

    /* Begins scheduling, until all processes have finished.
     * Prints the appropriate times at the end (Scheduler::print_times).
     */
    void start();

    /* Prints the times in the format:
     * <average turnaround time> t1 t2 t3 ... tn
     */
    void print_times() const;


private:
    typedef std::function<bool(std::shared_ptr<Process>, std::shared_ptr<Process>)> PIDComparator;

    PIDComparator comparator;
    std::set<std::shared_ptr<Process>, PIDComparator> processes;
    std::unordered_set<std::shared_ptr<Process>> ready_queue;
    std::map<int, int> stats;   // {pid: total_time}
    SchedulingAlgorithm* algorithm;
    std::shared_ptr<Process> current_job;

    /* Adds processes which have arrived at the current time,
     * and inserts them into the ready queue and scheduling algorithm.
     */
    void load();

    /* Updates the current_job pointer to the process corresponding to process_from_algorithm.
     */
    void set_current_process(const Process& process_from_algorithm);

    /* Obtains the next process to be run from the scheduling algorithm.
     * Updates the current_job pointer to that process.
     * Returns a reference to the process.
     */
    Process& sync_current_process();

    /* Pops the next process from the scheduling algorithm.
     * Throws an AlgorithmException if that process is not the same process pointed to by current_job.
     * Updates statistics on total time for that process.
     * Removes the process from the ready queue, and the global processes.
     */
    void remove_current_process();
};

#endif //PROJECT2_SCHEDULER_HPP
