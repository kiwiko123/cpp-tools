#ifndef PROJECT3_VIRTUAL_ADDRESS_HPP
#define PROJECT3_VIRTUAL_ADDRESS_HPP

#include <bitset>
#include <iostream>
#include <memory>
#include <cstdint>


class VirtualAddress
{
public:
    explicit VirtualAddress(int32_t addr);      // addr is a 32-bit int

    int32_t get_raw_address() const;
    int segment_number() const;
    int page_number() const;
    int offset() const;
    int segment_and_page_number() const;

private:
    int32_t raw_address;
    std::bitset<32> address;
    std::string discarded;

    std::string discard_bits(unsigned int discard) const;
};

#endif //PROJECT3_VIRTUAL_ADDRESS_HPP
