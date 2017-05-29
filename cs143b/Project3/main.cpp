#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdint>
#include "vm_system.hpp"



void process_page_table_line(std::ifstream& infile, VirtualMemorySystem& system)
{
    std::string line;
    std::getline(infile, line);
    std::istringstream processor{line};

    while (processor)
    {
        int segment;
        int address;
        processor >> segment;
        processor >> address;
        processor.sync();

        system.create_page_table(segment, address);
    }
}

void process_page_line(std::ifstream& infile, VirtualMemorySystem& system)
{
    std::string line;
    std::getline(infile, line);
    std::istringstream processor{line};

    while (processor)
    {
        int page;
        int segment;
        int address;
        processor >> page;
        processor >> segment;
        processor >> address;
        processor.sync();

        system.create_page(page, segment, address);
    }
}

void process_action_file(std::ifstream& infile, VirtualMemorySystem& system, bool tlb)
{
    std::string line;
    std::getline(infile, line);
    std::istringstream processor{line};

    while (processor)
    {
        int action;
        int32_t virtual_address;
        processor >> action;
        processor >> virtual_address;
        processor.sync();

        switch (action)
        {
            case 0:
                system.read(virtual_address, tlb);
                break;
            case 1:
                system.write(virtual_address, tlb);
                break;
            default:
                break;
        }
        std::cout.flush();
    }
}

void start(std::ifstream& memory_infile, std::ifstream& infile, VirtualMemorySystem& system, bool use_tlb)
{
    process_page_table_line(memory_infile, system);
    process_page_line(memory_infile, system);
    process_action_file(infile, system, use_tlb);
}


int main()
{
//    std::string memory_infile_path = "/Users/geoffreyko/OneDrive/School/2016-2017/CS 143B/Project3/tests/input1.txt";
//    std::string infile_path = "/Users/geoffreyko/OneDrive/School/2016-2017/CS 143B/Project3/tests/input2.txt";

//    std::string memory_infile_path = "/Users/geoffreyko/OneDrive/School/2016-2017/CS 143B/Project3/tests/my_input1.txt";
//    std::string infile_path = "/Users/geoffreyko/OneDrive/School/2016-2017/CS 143B/Project3/tests/my_input2.txt";

    std::string memory_infile_path = "/Users/geoffreyko/Downloads/input1.txt";
    std::string infile_path = "/Users/geoffreyko/Downloads/input2.txt";

//    std::string memory_infile_path = "/Volumes/cs143b/input1.txt";
//    std::string infile_path = "/Volumes/cs143b/input2.txt";

    std::string outfile1_path = "/Users/geoffreyko/OneDrive/School/2016-2017/CS 143B/Project3/tests/95975359-notlb.txt";
    std::string outfile2_path = "/Users/geoffreyko/OneDrive/School/2016-2017/CS 143B/Project3/tests/95975359-tlb.txt";
    std::ofstream outfile1{outfile1_path};
    std::ofstream outfile2{outfile2_path};

    std::ifstream memory_infile{memory_infile_path};
    std::ifstream infile{infile_path};

    auto buf1 = std::cout.rdbuf(outfile1.rdbuf());
    VirtualMemorySystem system;
    start(memory_infile, infile, system, false);

    std::cout.rdbuf(buf1);

    memory_infile.seekg(0);
    infile.seekg(0);

    auto buf2 = std::cout.rdbuf(outfile2.rdbuf());
    system = VirtualMemorySystem{};
    start(memory_infile, infile, system, true);

    std::cout.rdbuf(buf2);


    return 0;
}