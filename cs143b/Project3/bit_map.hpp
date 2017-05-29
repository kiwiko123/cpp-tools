#ifndef PROJECT3_BIT_MAP_HPP
#define PROJECT3_BIT_MAP_HPP

#include <bitset>
#include <iostream>
#include <memory>
#include <utility>

#define WIDTH 32

class BitMap
{
public:
    static int BITMAP_CAP;

    explicit BitMap(int cap);
    BitMap();

    friend std::ostream& operator<<(std::ostream& os, const BitMap& bm);

    int size() const;
    int width() const;
    std::pair<int, int> find_first_zero() const;
    std::pair<int, int> find_consecutive_zeros() const;

    void set(int index, int bit, bool value);
    void clear();

private:
    int capacity;
    std::unique_ptr<std::bitset<32>[]> array;

    std::pair<int, int> find_zero_starting_from(int index, int bit) const;
    void check_bounds(int& index, int& bit) const;
};

#endif //PROJECT3_BIT_MAP_HPP
