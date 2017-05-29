Process Scheduling Simulation
-----------------------------

This program simulates a CPU Scheduler, reading process times from an input file,
running them through four scheduling algorithms, and finally publishing their times into an output file.

The four algorithms implemented are:
  1) FIFO (First-In, First-Out)
     ----
     Processes execute in first-come, first-serve manner.
     ----
     If two processes arrive at the same time, the one which lexically appears first in the input file will run first.
     FIFO is a non-preemptive algorithm, so once a process starts, it will run to completion, at which point
     the next process will begin executing.
     This algorithm utilizes a queue as its data type to easily handle this first-come, first-serve nature.
     As a side-note, this implementation uses a std::deque, as opposed to a std::queue, for the sole reason that
     C++'s std::deque implements iterators, while std::queue does not.

  2) SJF (Shortest Job first)
     ------------------------
     Processes execute in the order of their run times.
     ------------------------
     If two processes have the same run times, they will execute in order of PID -
     that is, their lexical input file ordering.
     SJF is also a non-preemptive algorithm.
     This algorithm utilizes a priority queue to order processes based on run times,
     containing logic to prevent preemption when a new process arrives (and is added into the priority queue).

  3) SRT (Shortest Remaining Time first)
     -----------------------------------
     Processes execute in the order of their remaining times. Preemptive take on SJF.
     -----------------------------------
     Being the preemptive version of SJF, SRT employs a priority queue which orders processes
     purely on their remaining times.

  4) MLF (Multi-Level Feedback)
     --------------------------
     The most complex scheduling algorithm of the aforementioned three, MLF allows processes to take turns executing.
     Conceptually, there are several "levels" at which a process can reside.
     All processes start at the bottom level (0), and execute for 2^<level> CPU cycles -
       e.g., processes in level 0 execute for 1 cycle each, level 2 executes for 2, 3 for 4, etc.
     Once a process exhausts its designated quantum for that level, it moves up to the level, where it can execute
     for a longer time quantum.
     --------------------------
     As such, MLF is a preemptive algorithm.
     Implementing MLF the way that it conceptually functions is simple, yet somewhat unnecessary.
     To reduce space usage, my implementation employs a single priority queue which stores all the processes.
     A hash table (std::unordered_map) is used to associate time quantum and "level" information with each process.
     As such, the priority queue orders processes by their respective information, which is retrieved through
     constant time lookups from the hash table.
     This variation on MLF reduces the seemingly conventional dynamically-allocated array-of-array approach down to
     a single priority queue with a separate hash table.


The input files contain processes represented as 2-tuple pairs - (arrival time, run time).
  e.g., "0 1 0 4 2 3" indicates the following:
        - a process with run time of 1 arrived at time 0
        - a process with run time of 4 arrived at time 0
        - a process with run time of 3 arrived at time 2



Developed and tested on Apple LLVM version 7.3.0 (clang-703.0.31), C++14.

Author: Geoffrey Ko (2017)