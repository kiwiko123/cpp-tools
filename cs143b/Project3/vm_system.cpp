#include <sstream>
#include "vm_system.hpp"
#include "memory_exception.hpp"


const int VirtualMemorySystem::FRAME_SIZE = 512;
const int VirtualMemorySystem::N_FRAMES = 1024;
const int VirtualMemorySystem::PM_SIZE = FRAME_SIZE * N_FRAMES;
const int VirtualMemorySystem::ST_SIZE = FRAME_SIZE;
const int VirtualMemorySystem::PT_SIZE = FRAME_SIZE * 2;
const int VirtualMemorySystem::PAGE_SIZE = FRAME_SIZE;
const int VirtualMemorySystem::BM_SIZE = 32;
const int VirtualMemorySystem::READ_OP = 0;
const int VirtualMemorySystem::WRITE_OP = 1;

VirtualMemorySystem::VirtualMemorySystem()
    : physical_memory{new int[PM_SIZE]}, bit_map{BM_SIZE}
{
    init_physical_memory(0);
    bit_map.set(0, 0, true);    // ST resides in first frame
}

int VirtualMemorySystem::get_page_table(int segment, int page) const
{
    int pt_address = physical_memory[segment];
    return physical_memory[pt_address + page];
}

void VirtualMemorySystem::create_page_table(int segment, int address)
{
    // PM[s] -> start of PT
    physical_memory[segment] = address;

    // PT occupies 2 frames
    if (address != -1)
    {
        std::pair<int, int> frame = get_frame_number(address);
        bit_map.set(frame.first, frame.second, true);
        bit_map.set(frame.first, frame.second + 1, true);
    }
}

void VirtualMemorySystem::create_page(int page, int segment, int address)
{
    int page_table_address = physical_memory[segment];  // get index of PT start
    int page_start = page_table_address + page;         // PM[s] + p
    physical_memory[page_start] = address;              // PM[PM[s] + p]

    std::pair<int, int> frame = get_frame_number(address);
    bit_map.set(frame.first, frame.second, true);
}

void VirtualMemorySystem::read(int32_t address, bool use_tlb)
{
    VirtualAddress va{address};
    if (use_tlb)
    {
        tlb_operation(va, READ_OP);
    }
    else
    {
        read_no_tlb(va);
    }
}

void VirtualMemorySystem::write(int32_t address, bool use_tlb)
{
    VirtualAddress va{address};
    if (use_tlb)
    {
        tlb_operation(va, WRITE_OP);
    }
    else
    {
        write_no_tlb(va);
    }
}

bool VirtualMemorySystem::read_no_tlb(const VirtualAddress& va) const
{
    int s = va.segment_number();
    int p = va.page_number();
    int segment_table = physical_memory[s];
    int page_table = get_page_table(s, p);  // PM[PM[s] + p]
    bool valid_address = false;

    if (segment_table == -1 || page_table == -1)
    {
        std::cout << "pf";
    }
    else if (segment_table == 0 || page_table == 0)
    {
        std::cout << "err";
    }
    else
    {
        valid_address = true;
        std::cout << page_table + va.offset();
    }
    std::cout << " ";
    return valid_address;
}

bool VirtualMemorySystem::write_no_tlb(const VirtualAddress& va)
{
    int s = va.segment_number();
    int p = va.page_number();
    int segment_table_entry = physical_memory[s];
    int page_table_entry = get_page_table(s, p);
    bool valid_address = false;

    if (segment_table_entry == -1 || page_table_entry == -1)
    {
        std::cout << "pf";
    }
    else if (segment_table_entry == 0)
    {
        // allocate new blank PT
        allocate_new_page_table(s);
        return write_no_tlb(va);
    }
    else if (page_table_entry == 0)
    {
        // create a new blank page
        allocate_new_page(s, p);
        return write_no_tlb(va);
    }
    else
    {
        valid_address = true;
        std::cout << page_table_entry + va.offset();
    }
    std::cout << " ";
    return valid_address;
}

void VirtualMemorySystem::tlb_operation(const VirtualAddress& va, int operation)
{
    int sp = va.segment_and_page_number();
    int hit_index = tlb.hit_index(sp);

    if (hit_index == -1)
    {
        // miss
        std::cout << "m ";
        bool valid_address = operation == READ_OP ? read_no_tlb(va) : write_no_tlb(va);
        if (valid_address)
        {
            int s = va.segment_number();
            int p = va.page_number();
            int frame = physical_memory[physical_memory[s] + p];
            tlb.do_miss(sp, frame);
        }
    }
    else
    {
        // hit
        std::cout << "h ";
        int f = tlb.do_hit(sp, hit_index);
        std::cout << f + va.offset() << " ";
    }
}

void VirtualMemorySystem::init_physical_memory(int fill_value)
{
    for (unsigned int i = 0; i < PM_SIZE; ++i)
    {
        physical_memory[i] = fill_value;
    }
}

std::pair<int, int> VirtualMemorySystem::get_frame_number(int physical_address) const
{
    const int index_capacity = FRAME_SIZE * bit_map.size();

    int frame = physical_address / index_capacity;
    int bit =  physical_address / FRAME_SIZE % bit_map.width();

    return std::make_pair(frame, bit);
}

void VirtualMemorySystem::allocate_new_page_table(int segment)
{
    std::pair<int, int> location = bit_map.find_consecutive_zeros();
    const int index_capacity = FRAME_SIZE * bit_map.size();
    int page_table_address = location.first * index_capacity + location.second * FRAME_SIZE;

    if (physical_memory[segment] != 0)
    {
        std::ostringstream buf;
        buf << "VirtualMemorySystem::allocate_new_page_table - ";
        buf << "PM[" << segment << "] is not 0 (" << physical_memory[segment] << ")";
        throw MemoryException{buf.str()};
    }

    physical_memory[segment] = page_table_address;
    if (page_table_address != -1)
    {
        bit_map.set(location.first, location.second, true);
        bit_map.set(location.first, location.second + 1, true);
    }
}

void VirtualMemorySystem::allocate_new_page(int segment, int page)
{
    std::pair<int, int> location = bit_map.find_first_zero();
    const int index_capacity = FRAME_SIZE * bit_map.size();
    int next_free_address = location.first * index_capacity + location.second * FRAME_SIZE;

    int page_table_address = physical_memory[segment];
    int page_start = page_table_address + page;

    physical_memory[page_start] = next_free_address;
    bit_map.set(location.first, location.second, true);
}

void VirtualMemorySystem::clear()
{
    init_physical_memory(0);
    bit_map.clear();
}