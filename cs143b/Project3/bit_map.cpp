#include <iomanip>
#include <iostream>
#include <sstream>
#include "bit_map.hpp"
#include "memory_exception.hpp"


int BitMap::BITMAP_CAP = 32;

BitMap::BitMap(int cap)
    : capacity{cap}, array{new std::bitset<32>[cap]}
{
    clear();
}

std::ostream& operator<<(std::ostream& os, const BitMap& bm)
{
    os << "BitMap(" << std::endl;
    for (unsigned int i = 0; i < bm.size(); ++i)
    {
        os << "  " << std::setw(2) << i << ": " << bm.array[i] << std::endl;
    }
    os << ")";
    return os;
}

int BitMap::size() const
{
    return capacity;
}

int BitMap::width() const
{
    return WIDTH;
}

void BitMap::clear()
{
    for (unsigned int i = 0; i < size(); ++i)
    {
        array[i].reset();
    }
}

std::pair<int, int> BitMap::find_first_zero() const
{
    return find_zero_starting_from(0, 1);   // skip ST (0, 0)
}

std::pair<int, int> BitMap::find_consecutive_zeros() const
{
    int index = 0;
    int bit = 1;

    while (index < size())
    {
        std::pair<int, int> first_pair = find_zero_starting_from(index, bit);
        int first = first_pair.first;
        int second = first_pair.second + 1;
        check_bounds(first, second);

        if (!array[first].test(second))
        {
            return first_pair;
        }
        index = first;
        bit = second + 1;
        check_bounds(index, bit);
    }
    throw MemoryException{"BitMap::find_consecutive_zeros - bitmap out of space"};
}

void BitMap::set(int index, int bit, bool value)
{
    check_bounds(index, bit);
    array[index].set(bit, value);
}

std::pair<int, int> BitMap::find_zero_starting_from(int index, int bit) const
{
    if (index >= size())
    {
        std::ostringstream msg;
        msg << "BitMap::find_zero_starting_from - " << index << " out of range";
        throw std::out_of_range{msg.str()};
    }
    if (bit == width())
    {
        bit = 0;
    }

    for (int i = index; i < size(); ++i)
    {
        bool first_iteration = i == index;
        for (int j = first_iteration ? bit : 0; j < width(); ++j)
        {
            if (!array[i].test(j))
            {
                return std::make_pair(i, j);
            }
        }
    }
    throw MemoryException{"BitMap::find_zero_starting_from - bitmap out of space"};
}

void BitMap::check_bounds(int& index, int& bit) const
{
    if (bit == width())
    {
        bit = 0;
        ++index;
    }
}