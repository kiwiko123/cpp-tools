// A map is a data type that associates a collection of key-value pairs.
// Using a separately chained hash table as its underlying data structure,
// we can efficiently implement maps that have (on average) O(1) operations.
//
// The hash table is composed of a calculated number of LinkedList objects,
// which adds more as necessary.
#ifndef DATA_STRUCTURES_HASH_MAP_HPP
#define DATA_STRUCTURES_HASH_MAP_HPP

#include <iostream>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include <cstdlib>
#include "linked_list_old.hpp"
#include "pair.hpp"


namespace
{
    double _HASH_MAP_LOAD_FACTOR_THRESHOLD = 1.0;
    int _HASH_MAP_INITIAL_SIZE = 5;
}



template <typename KEY, typename VALUE>
class HashMap
{
private:
    typedef pair<KEY, VALUE> Entry;

public:
    HashMap(std::function<int(const KEY&)> hasher, double the_load_factor);
    explicit HashMap(std::function<int(const KEY&)> hasher);
    HashMap(const HashMap& right);
    ~HashMap();


    // Operators
    HashMap<KEY, VALUE>& operator=(const HashMap<KEY, VALUE>& right);
    const VALUE& operator[](const KEY& key) const;
    VALUE& operator[](const KEY& key);

    /* Two hash_maps are == if they have the same keys with the same values associated with those keys.
     * Two hash_maps do not have to have the same hash function to be ==.
     */
    bool operator==(const HashMap<KEY, VALUE>& right) const;
    bool operator!=(const HashMap<KEY, VALUE>& right) const;

    template <typename K, typename V>
    friend std::ostream& operator<<(std::ostream& os, const HashMap<K, V>& hm);

    // Member Functions
    int size() const;
    bool empty() const;
    bool contains(const KEY& key) const;
    std::string str() const;

    /* Returns a std::vector of all keys/values in the map */
    std::vector<KEY> keys() const;
    std::vector<VALUE> values() const;
    std::vector<Entry> items() const;

    // Modifying Member Functions
    /* Inserts (key, value) into the map.
     * If the key is already in the map,
     * the existing value associated with it is replaced with this value.
     */
    void push_back(const KEY& key, const VALUE& value);
    void push_back(const Entry& pair);

    /* Removes the {key: value} association from the map */
    void erase(const KEY& key);

    /* Empties the map */
    void clear();


    class iterator;
    auto begin() const -> iterator;
    auto end() const -> iterator;

    /* Iteration through a map produces its keys.
     * Values can be accessed through iteration by using operator[].
     * If both keys and values are instantly needed, iterate through hash_map::items().
     */
    class iterator
    {
    public:
        iterator();

        auto operator++() -> iterator&;
        auto operator++(int) -> iterator;
        bool operator==(const iterator& right) const;
        bool operator!=(const iterator& right) const;
        KEY& operator*() const;
        KEY* operator->() const;

        friend iterator HashMap<KEY, VALUE>::begin() const;
        friend iterator HashMap<KEY, VALUE>::end() const;

    private:
        iterator(HashMap<KEY, VALUE>* it, int already);

        bool done() const;
        void advance_bin();
        void next();

        HashMap<KEY, VALUE>* ref;
        int traversed;
        typename LinkedList<Entry>::iterator current;
        int current_bin_index;
    };


protected:
    std::function<int(const KEY&)> hash;
    int bins;
    LinkedList<Entry>* table;


private:
    double lft;
    unsigned int length;

    void _rehash();
    double load_factor() const;
    int get_bin(const KEY& key) const;
    Entry* _locate(const KEY& key) const;
};


template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>::HashMap(std::function<int(const KEY&)> hasher, double the_load_factor)
        : hash{hasher}, bins{_HASH_MAP_INITIAL_SIZE}, table{new LinkedList<Entry>[bins]}, lft{the_load_factor}, length{0}
{
}

template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>::HashMap(std::function<int(const KEY&)> hasher) : HashMap(hasher, _HASH_MAP_LOAD_FACTOR_THRESHOLD)
{
}

template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>::HashMap(const HashMap<KEY, VALUE>& right)
        : hash{right.hash}, bins{right.bins}, table{new LinkedList<Entry>[bins]}, lft{right.lft}, length{right.length}
{
    for (unsigned int i = 0; i < right.bins; ++i)
        table[i] = right.table[i];
}

template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>::~HashMap()
{
    delete[] table;
}

template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>& HashMap<KEY, VALUE>::operator=(const HashMap<KEY, VALUE>& right)
{
    if (this != &right)
    {
        delete[] table;
        hash = right.hash;
        bins = right.bins;
        lft = right.lft;
        length = right.length;
        table = new LinkedList<Entry>[bins];

        for (unsigned int i = 0; i < bins; ++i)
            table[i] = right.table[i];
    }
    return *this;
}

template <typename KEY, typename VALUE>
const VALUE& HashMap<KEY, VALUE>::operator[](const KEY& key) const
{
    if (!contains(key))
        throw std::invalid_argument{"key not in map"};

    return _locate(key)->second;
}

template <typename KEY, typename VALUE>
VALUE& HashMap<KEY, VALUE>::operator[](const KEY& key)
{
    if (!contains(key))
        push_back(key, VALUE{});

    return _locate(key)->second;
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::operator==(const HashMap<KEY, VALUE>& right) const
{
    if (this == &right)
        return true;
    if (size() != right.size())
        return false;

    for (unsigned int i = 0; i < right.bins; ++i)
    {
        for (const auto& entry : right.table[i])
        {
            if (!(contains(entry.first) && entry.second == operator[](entry.first)))
                return false;
        }
    }
    return true;
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::operator!=(const HashMap<KEY, VALUE>& right) const
{
    return !operator==(right);
}

template <typename KEY, typename VALUE>
std::ostream& operator<<(std::ostream& os, const HashMap<KEY, VALUE>& hm)
{
    os << "hash_map(";
    unsigned int c = 0;
    for (unsigned int i = 0; i < hm.bins; ++i)
    {
        for (const auto& entry : hm.table[i])
            os << entry.first << ": " << entry.second << (c++ < hm.size() - 1 ? ", " : "");
    }
    os << ")";

    return os;
}

template <typename KEY, typename VALUE>
int HashMap<KEY, VALUE>::size() const
{
    return length;
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::empty() const
{
    return size() == 0;
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::contains(const KEY& key) const
{
    return _locate(key) != nullptr;
}

template <typename KEY, typename VALUE>
std::string HashMap<KEY, VALUE>::str() const
{
    std::ostringstream result;
    result << "hash_map(" << std::endl;
    for (unsigned int i = 0; i < bins; ++i)
        result << "  " << i << ": " << table[i] << std::endl;

    result << ")";
    return result.str();
}

template <typename KEY, typename VALUE>
std::vector<KEY> HashMap<KEY, VALUE>::keys() const
{
    std::vector<KEY> result;
    for (unsigned int i = 0; i < bins; ++i)
    {
        for (const auto& entry : table[i])
            result.push_back(entry.first);
    }
    return result;
}

template <typename KEY, typename VALUE>
std::vector<VALUE> HashMap<KEY, VALUE>::values() const
{
    std::vector<VALUE> result;
    for (unsigned int i = 0; i < bins; ++i)
    {
        for (const auto& entry : table[i])
            result.push_back(entry.second);
    }
    return result;
}

template <typename KEY, typename VALUE>
std::vector<typename HashMap<KEY, VALUE>::Entry> HashMap<KEY, VALUE>::items() const
{
    std::vector<Entry> result;
    for (unsigned int i = 0; i < bins; ++i)
    {
        for (const auto& entry : table[i])
            result.push_back(entry);
    }
    return result;
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::push_back(const KEY& key, const VALUE& value)
{
    if (contains(key))
        erase(key);
    if (load_factor() >= lft)
        _rehash();

    table[get_bin(key)].push_front(Entry{key, value});
    ++length;
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::push_back(const typename HashMap<KEY, VALUE>::Entry& pair)
{
    push_back(pair.first, pair.second);
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::erase(const KEY& key)
{
    if (!contains(key))
        throw std::invalid_argument{"key is not in map"};

    table[get_bin(key)].erase(Entry{key, operator[](key)});
    --length;
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::clear()
{
    delete[] table;
    length = 0;
    bins = _HASH_MAP_INITIAL_SIZE;
    table = new LinkedList<Entry>[bins];
}

template <typename KEY, typename VALUE>
double HashMap<KEY, VALUE>::load_factor() const
{
    return size() / static_cast<double>(bins);
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::_rehash()
{
    int old_bins = bins;
    bins *= 2;
    LinkedList<Entry>* reallocated = new LinkedList<Entry>[bins];
    for (unsigned int i = 0; i < old_bins; ++i)
    {
        for (const auto& entry : table[i])
            reallocated[get_bin(entry.first)].push_back(entry);
    }

    delete[] table;
    table = reallocated;
}

template <typename KEY, typename VALUE>
int HashMap<KEY, VALUE>::get_bin(const KEY& key) const
{
    return std::abs(hash(key)) % bins;
}

template <typename KEY, typename VALUE>
typename HashMap<KEY, VALUE>::Entry* HashMap<KEY, VALUE>::_locate(const KEY& key) const
{
    for (auto& entry : table[get_bin(key)])
    {
        if (entry.first == key)
            return &entry;
    }
    return nullptr;
}


// iterator implementation
template <typename KEY, typename VALUE>
auto HashMap<KEY, VALUE>::begin() const -> HashMap<KEY, VALUE>::iterator
{
    return iterator{const_cast<HashMap<KEY, VALUE>*>(this), 0};
}

template <typename KEY, typename VALUE>
auto HashMap<KEY, VALUE>::end() const -> HashMap<KEY, VALUE>::iterator
{
    return iterator{const_cast<HashMap<KEY, VALUE>*>(this), size()};
}

template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>::iterator::iterator(HashMap<KEY, VALUE>* it, int already)
        : ref{it}, traversed{already}, current_bin_index{-1}
{
    advance_bin();
}

template <typename KEY, typename VALUE>
HashMap<KEY, VALUE>::iterator::iterator() : iterator(nullptr, 0)
{
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::iterator::done() const
{
    return traversed >= ref->size();
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::iterator::advance_bin()
{
    for (++current_bin_index; current_bin_index < ref->bins && ref->table[current_bin_index].empty(); ++current_bin_index)
    { /* advance current_bin_index to next non-empty bin */ }
    current = ref->table[current_bin_index].begin();
}

template <typename KEY, typename VALUE>
void HashMap<KEY, VALUE>::iterator::next()
{
    if (!done())
    {
        try
        {
            *(++current);
        }
        catch (std::out_of_range& e)
        {
            advance_bin();
        }
        ++traversed;
    }
}

template <typename KEY, typename VALUE>
auto HashMap<KEY, VALUE>::iterator::operator++() -> HashMap<KEY, VALUE>::iterator&
{
    if (!done())
        next();

    return *this;
}

template <typename KEY, typename VALUE>
auto HashMap<KEY, VALUE>::iterator::operator++(int) -> HashMap<KEY, VALUE>::iterator
{
    iterator to_return{*this};
    operator++();

    return to_return;
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::iterator::operator==(const HashMap<KEY, VALUE>::iterator& right) const
{
    return ref == right.ref && traversed == right.traversed;
}

template <typename KEY, typename VALUE>
bool HashMap<KEY, VALUE>::iterator::operator!=(const HashMap<KEY, VALUE>::iterator& right) const
{
    return !operator==(right);
}

template <typename KEY, typename VALUE>
KEY& HashMap<KEY, VALUE>::iterator::operator*() const
{
    if (done())
        throw std::out_of_range{"hash_map::iterator::operator* -- cursor past end"};

    return current->first;
}

template <typename KEY, typename VALUE>
KEY* HashMap<KEY, VALUE>::iterator::operator->() const
{
    if (done())
        throw std::out_of_range{"hash_map::iterator::operator-> -- cursor past end"};

    return &current->first;
}


#endif // DATA_STRUCTURES_HASH_MAP_HPP
