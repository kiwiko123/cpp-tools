#ifndef PROJECT3_VM_SYSTEM_HPP
#define PROJECT3_VM_SYSTEM_HPP

#include <iostream>
#include <memory>
#include <utility>
#include <cstdint>
#include "virtual_address.hpp"
#include "bit_map.hpp"
#include "tlb.hpp"


class VirtualMemorySystem
{
public:
    // -------------------------
    static const int FRAME_SIZE;
    static const int N_FRAMES;
    static const int PM_SIZE;
    static const int ST_SIZE;
    static const int PT_SIZE;
    static const int PAGE_SIZE;
    static const int BM_SIZE;
    static const int BM_INDEX_CAP;
    static const int READ_OP;
    static const int WRITE_OP;
    // -------------------------

    VirtualMemorySystem();

    int get_page_table(int segment, int page) const;

    void create_page_table(int segment, int address);
    void create_page(int page, int segment, int address);

    void read(int32_t address, bool use_tlb);
    void write(int32_t address, bool use_tlb);

    void clear();


private:
    // --- Private Members ---
    std::unique_ptr<int[]> physical_memory;
    BitMap bit_map;
    TranslationLookAsideBuffer tlb;
    // -----------------------

    // --- Private Helper Functions ---
    void init_physical_memory(int fill_value);
    std::pair<int, int> get_frame_number(int physical_address) const;

    bool read_no_tlb(const VirtualAddress& va) const;
    bool write_no_tlb(const VirtualAddress& va);
    void tlb_operation(const VirtualAddress& va, int operation);

    void allocate_new_page_table(int segment);
    void allocate_new_page(int segment, int page);
    // --------------------------------
};

#endif //PROJECT3_VM_SYSTEM_HPP
