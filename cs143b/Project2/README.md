# Process Scheduling Simulation
This program simulates a CPU Scheduler, reading process times from an input file, running them through four scheduling algorithms, then finally publishing their times into an output file.

## The Algorithms
### FIFO (First-In, First-Out)
#### Processes execute in a first-come, first-serve manner.
If two processes arrive at the same time, the one which lexically appears first in the input file will run first. FIFO is a non-preemptive algorithm, so once a process starts, it will run to completion, at which point the next process will begin executing. This algorithm utilizes a queue as its data type to easily handle this first-come, first-serve nature. As a side-note, this implementation uses `std::deque`, as opposed to `std::queue`, for the sole reason that C++'s `std::deque` implements iterators, while `std::queue` does not.

### SJF (Shortest Job First)
#### Processes execute in the order of their run times.
If two processes have the same run times, they will execute in order of PID - that is, their lexical input file ordering. SJF is also a non-preemptive algorithm. It utilizes a priority queue to order processes based on run times, containing logic to prevent preemption when a new process arrives (and is added into the priority queue).

### SRT (Shortest Remaining Time)
#### Processes execute in the order of their remaining times.
This is a preemptive take on SJF. SRT employs a priority queue which orders processes purely on their remaining times.

### MLF (Multi-Level Feedback)
#### Processes take turns, increasing the amount of execution time after each round.
The most complex scheduling algorithm of the four, MLF allows processes to take turns executing. Conceptually, there are several "levels" at which a process can reside. All processes start at the bottom level (0), and execute for 2<sup>*level*</sup> CPU cycles - e.g., processes in level 0 execute for 1 cycle each, those in level 1 execute for 2, 2 for 4, 3 for 8, etc. Once a process exhausts its designated quantum at level *i*, it moves up to the next level *i + 1*, where it can execute for a longer time quantum (2<sup>*i + 1*</sup>) after all other processes in level *i* have gone. As such, MLF is a preemptive algorithm.
Implementing MLF the way that it conceptually works is simple. Intuitively, you might have an array-of-arrays, where each inner array represents each level. Within each inner array resides the processes at that level. However, this requires a potentially large amount of space, especially if one process has a significantly longer running time than any other.
To reduce space usage, my implementation employs a single priority queue which stores all the processes. A hash table is used to associate time quantum and "level" information with each process. The priority queue orders its processes by their respective information, which is retrieved through constant-time lookups from the hash table. This variation on MLF reduces the conventional array-of-array approach down to a single priority queue with an auxiliary hash table.

## Reading the Input Files
The input files contain processes represented as 2-tuple pairs - (arrival time, run time).
For example, "0 1 0 4 2 3" indicates the following:

- a process with a run time of 1 arrives at time 0
- a process with a run time of 4 arrives at time 0
- a process with a run time of 3 arrives at time 2

## Notes
Author: Geoffrey Ko (2017)
Developed and tested on Apple LLVM version 7.3.0 (clang-703.0.31), C++14.