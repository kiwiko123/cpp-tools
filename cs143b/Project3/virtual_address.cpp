#include "virtual_address.hpp"


VirtualAddress::VirtualAddress(int32_t addr)
    : raw_address{addr}, address{static_cast<unsigned long long>(addr)}, discarded{discard_bits(4)}
{
}

int32_t VirtualAddress::get_raw_address() const
{
    return raw_address;
}

int VirtualAddress::segment_number() const
{
    std::string segment_slice = discarded.substr(0, 9);
    std::bitset<9> bits{segment_slice};
    return bits.to_ullong();
}

int VirtualAddress::page_number() const
{
    std::string page_slice = discarded.substr(9, 10);
    std::bitset<10> bits{page_slice};
    return bits.to_ullong();
}

int VirtualAddress::offset() const
{
    std::string offset_slice = discarded.substr(19, 9);
    std::bitset<9> bits{offset_slice};
    return bits.to_ullong();
}

int VirtualAddress::segment_and_page_number() const
{
    std::string sp_slice = discarded.substr(0, 9 + 10);
    std::bitset<19> bits{sp_slice};
    return bits.to_ullong();
}

std::string VirtualAddress::discard_bits(unsigned int discard) const
{
    std::string bits = address.to_string();
    return bits.substr(discard);
}