#ifndef PROJECT2_PROCESS_HPP
#define PROJECT2_PROCESS_HPP

#include <iostream>


class Process
{
public:
    Process(int process_id, int ar_time, int r_time);

    friend std::ostream& operator<<(std::ostream& os, const Process& p);

    /* Returns true if other has the same pid as this */
    bool operator==(const Process& other) const;
    bool operator!=(const Process& other) const;

    /* Runs the process for 1 "cycle";
     * If blocked, increments waiting time, and returns -1.
     * Updates stats if finished.
     * Returns the updated current time.
     */
    int operator++();

    /* Accessors */
    int id() const;
    int arrival_time() const;
    int run_time() const;
    int total_time() const;
    int waiting_time() const;
    int current_time() const;
    int remaining_time() const;

    /* Returns true if this process has arrived;
     * e.g., if arrival time is less than the total (static) current time.
     */
    bool arrived() const;

    /* Returns true if the process has finished */
    bool finished() const;

    /* Returns true if the process will finish in the next clock cycle */
    bool finishes_next() const;

    /* Returns true if the process is blocked */
    bool is_blocked() const;

    /* Blocks the current process from running */
    void block();

    /* Unblocks the current process */
    void unblock();

    /* Increments the process' waiting time by 1 cycle */
    void wait();


    /* Returns the current, global clock cycle number */
    static int clock_time();

    /* Increments the global clock cycle number */
    static int increment_clock_time();

    /* Resets the global clock cycle to 0 */
    static void reset_clock_time();


private:
    int pid;
    int arrival;
    int run;
    int waiting;
    int current;

    bool done;
    bool blocked;

    static int c_time;
};

#endif //PROJECT2_PROCESS_HPP
