#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
#include <string>
#include <cctype>
#include "scheduler.hpp"
#include "fifo_algorithm.hpp"
#include "sjf_algorithm.hpp"
#include "srt_algorithm.hpp"
#include "mlf_algorithm.hpp"


std::vector<std::pair<int, int>> process_file(std::ifstream& file)
{
    std::vector<std::pair<int, int>> result;

    for (std::string line; std::getline(file, line);)
    {
        std::istringstream processor{line};
        while (processor)
        {
            char c = processor.peek();
            if (!(std::isspace(c) || std::isdigit(c)))
            {
                break;
            }
            int arrival_time, total_time;
            processor >> arrival_time;
            processor >> total_time;

            result.push_back(std::make_pair(arrival_time, total_time));
        }
        break;  // input file is 1 line long
    }

    return result;
}


template <typename Algorithm>
Scheduler create()
{
    return Scheduler{new Algorithm};
}

template <typename Algorithm>
void create_from_file(std::ifstream& file)
{
    std::vector<std::pair<int, int>> process_info = process_file(file);

    Scheduler s = create<Algorithm>();
    for (const auto& pair: process_info)
    {
        s.read_process(pair.first, pair.second);
    }
    s.start();
}

void run_all_from_file(std::ifstream& file)
{
    std::vector<std::pair<int, int>> process_info = process_file(file);
    Scheduler fifo = create<FIFOAlgorithm>();
    Scheduler sjf = create<SJFAlgorithm>();
    Scheduler srt = create<SRTAlgorithm>();
    Scheduler mlf = create<MLFAlgorithm>();

    for (const auto& pair: process_info)
    {
        fifo.read_process(pair.first, pair.second);
        sjf.read_process(pair.first, pair.second);
        srt.read_process(pair.first, pair.second);
        mlf.read_process(pair.first, pair.second);
    }

    fifo.start();
    sjf.start();
    srt.start();
    mlf.start();
}


int main()
{
    std::string file_path = "tests/sample_input.txt";
    std::ifstream infile{file_path};

    std::string outfile_path = "tests/__OUT.txt";
    std::ofstream outfile{outfile_path};

    // -----------------------------------------
    // redirect std::cout to outfile
    auto buf = std::cout.rdbuf(outfile.rdbuf());

    run_all_from_file(infile);

    // restore std::cout
    std::cout.rdbuf(buf);

    return 0;
}