#ifndef PROJECT3_TLB_HPP
#define PROJECT3_TLB_HPP

#include <iostream>
#include <memory>

class TranslationLookAsideBuffer
{
public:
    explicit TranslationLookAsideBuffer(int max_size);
    TranslationLookAsideBuffer();

    int hit_index(int sp) const;
    int max_lru_value() const;

    int do_hit(int sp, int line);
    void do_miss(int sp, int frame);

private:
    struct Entry
    {
        int lru_value;
        int sp;
        int f;
    };

    int capacity;
    std::unique_ptr<Entry[]> cache;
    int length;

    void init_cache();
    int find_index_with_value(int value) const;
};

#endif //PROJECT3_TLB_HPP
