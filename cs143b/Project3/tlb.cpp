#include <algorithm>
#include "tlb.hpp"


TranslationLookAsideBuffer::TranslationLookAsideBuffer(int max_size)
    : capacity{max_size}, cache{new Entry[capacity]}, length{0}
{
    init_cache();
}

TranslationLookAsideBuffer::TranslationLookAsideBuffer()
    : TranslationLookAsideBuffer{4}
{
}

int TranslationLookAsideBuffer::hit_index(int sp) const
{
    for (unsigned int i = 0; i < capacity; ++i)
    {
        Entry& entry = cache[i];
        if (cache[i].sp == sp)
        {
            return i;
        }
    }
    return -1;
}

int TranslationLookAsideBuffer::max_lru_value() const
{
    return capacity - 1;
}

int TranslationLookAsideBuffer::do_hit(int sp, int line)
{
    Entry& hit = cache[line];
    for (unsigned int i = 0; i < capacity; ++i)
    {
        Entry& current = cache[i];
        if (current.lru_value < hit.lru_value)
        {
            --(current.lru_value);
        }
    }
    hit.lru_value = max_lru_value();
    return hit.f;
}

void TranslationLookAsideBuffer::do_miss(int sp, int frame)
{
    int lowest_lru_index = find_index_with_value(0);
    Entry& entry = cache[lowest_lru_index];
    entry.lru_value = max_lru_value();
    entry.sp = sp;
    entry.f = frame;

    for (unsigned int i = 0; i < capacity; ++i)
    {
        if (i != lowest_lru_index)
        {
            --(cache[i].lru_value);
        }
    }
}

int TranslationLookAsideBuffer::find_index_with_value(int value) const
{
    int i = 0;
    for (; i < capacity; ++i)
    {
        if (cache[i].lru_value == value)
        {
            break;
        }
    }
    return i;
}

void TranslationLookAsideBuffer::init_cache()
{
    for (unsigned int i = 0; i < capacity; ++i)
    {
        cache[i] = Entry{0, -1, -1};
    }
}