#ifndef DATA_STRUCTURES_LRU_MAP_HPP
#define DATA_STRUCTURES_LRU_MAP_HPP

#include <string>
#include <unordered_map>
#include "lru_cache.hpp"

#define DEBUG true

template <typename Key, typename T>
class LRUMap : public LRUCache<T>
{
public:
    explicit LRUMap(int cap);
    LRUMap();

    typename LRUCache<T>::NodePointer operator[](const Key& key);

    virtual bool empty() const override;
    virtual int size() const override;
    bool contains(const Key& key) const;

    void push(const Key& key, const T& value);
    void erase(const Key& key);

    const T& touch(const Key& key);

protected:
    virtual std::string name() const override;

private:
    std::unordered_map<Key, typename LRUCache<T>::NodePointer> table;
    int capacity;   // not yet functional

    bool full() const;
    void repush(const Key& key);
    void pop_least_recently_used();
};


template <typename Key, typename T>
LRUMap<Key, T>::LRUMap(int cap) : LRUCache<T>{}
{
    capacity = cap;
}

template <typename Key, typename T>
LRUMap<Key, T>::LRUMap() : LRUCache<T>{}
{
    capacity = -1;
}

template <typename Key, typename T>
typename LRUCache<T>::NodePointer LRUMap<Key, T>::operator[](const Key& key)
{
    return touch(key);
}

template <typename Key, typename T>
std::string LRUMap<Key, T>::name() const
{
    return "LRUMap";
}

template <typename Key, typename T>
bool LRUMap<Key, T>::empty() const
{
    return table.empty();
}

template <typename Key, typename T>
int LRUMap<Key, T>::size() const
{
    return table.size();
}

template <typename Key, typename T>
bool LRUMap<Key, T>::contains(const Key& key) const
{
    return table.find(key) != table.end();
};

template <typename Key, typename T>
void LRUMap<Key, T>::push(const Key& key, const T& value)
{
    if (contains(key))
    {
        repush(key);
    }
    else
    {
        auto node = this->push_back_node(value);
        table[key] = node;
    }
}

template <typename Key, typename T>
void LRUMap<Key, T>::erase(const Key& key)
{
    if (!contains(key))
    {
        throw std::runtime_error{"LRUMap<Key, T>::erase - key not found"};
    }
    auto target = table.at(key);
    this->erase_node(target);
    table.erase(key);
}

template <typename Key, typename T>
void LRUMap<Key, T>::repush(const Key& key)
{
    const T& value = table.at(key)->value;
    erase(key);
    push(key, value);
}

template <typename Key, typename T>
bool LRUMap<Key, T>::full() const
{
    return capacity >= 0 && size() >= capacity;
}

template <typename Key, typename T>
const T& LRUMap<Key, T>::touch(const Key& key)
{
    if (!contains(key))
    {
        throw std::runtime_error{"LRUMap<Key, T>::touch - key not found"};
    }
    const T& value = table.at(key)->value;
    repush(key);
    return value;
}

template <typename Key, typename T>
void LRUMap<Key, T>::pop_least_recently_used()
{
}

#undef DEBUG
#endif // DATA_STRUCTURES_LRU_MAP_HPP