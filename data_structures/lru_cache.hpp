#ifndef DATA_STRUCTURES_LRU_CACHE_HPP
#define DATA_STRUCTURES_LRU_CACHE_HPP

#include <iostream>
#include <string>
#include <unordered_map>
#include "cache_linked_list.hpp"

#ifndef DEBUG
#define DEBUG true
#endif
#define LOGME(m) if (DEBUG) { std::cout << __FILE__ << std::endl << "  " << __func__ << std::endl << "    \"" << (m) << "\"" << std::endl; }


template <typename Key, typename T>
class LRUCache
{
public:
    LRUCache();

	template <typename K, typename V>
	friend std::ostream& operator<<(std::ostream& os, const LRUCache<K, V>& map);
	T& operator[](const Key& key);

    bool empty() const;
    int size() const;
    bool contains(const Key& key) const;

    void push(const Key& key, const T& value);
    void erase(const Key& key);

    const T& get(const Key& key);


private:
    std::unordered_map<Key, typename CacheLinkedList<T>::NodePointer> table;
    CacheLinkedList<T> list;

    void repush(const Key& key);
};


template <typename Key, typename T>
LRUCache<Key, T>::LRUCache()
{
}

template <typename Key, typename T>
std::ostream& operator<<(std::ostream& os, const LRUCache<Key, T>& map)
{
	os << "LRUCache(";
	if (!map.empty())
	{
		auto i = map.list.cbegin();
		os << *(i++);
		while (i != map.list.cend())
		{
			os << ", " << *(i++);
		}
	}
	os << ")";
	return os;
}

template <typename Key, typename T>
T& LRUCache<Key, T>::operator[](const Key& key)
{
	LOGME("pushing");
	if (contains(key))
	{
		push(key, T{});
	}
	else
	{
		get(key);
	}
	return table.at(key)->value;
}

template <typename Key, typename T>
bool LRUCache<Key, T>::empty() const
{
    return table.empty();
}

template <typename Key, typename T>
int LRUCache<Key, T>::size() const
{
    return table.size();
}

template <typename Key, typename T>
bool LRUCache<Key, T>::contains(const Key& key) const
{
    return table.find(key) != table.end();
};

template <typename Key, typename T>
void LRUCache<Key, T>::push(const Key& key, const T& value)
{
    if (contains(key))
    {
        repush(key);
    }
    else
    {
        table[key] = list.push_back(value);
    }
}

template <typename Key, typename T>
void LRUCache<Key, T>::erase(const Key& key)
{
    if (!contains(key))
    {
        throw std::runtime_error{"LRUCache<Key, T>::erase - key not found"};
    }
    auto target = table.at(key);
    list.erase_node(target);
    table.erase(key);
}

template <typename Key, typename T>
void LRUCache<Key, T>::repush(const Key& key)
{
	const T& value = table.at(key)->value;
    erase(key);
    push(key, value);
}

template <typename Key, typename T>
const T& LRUCache<Key, T>::get(const Key& key)
{
    if (!contains(key))
    {
        throw std::runtime_error{"LRUCache<Key, T>::get - key not found"};
    }
    const T& value = table.at(key)->value;
    repush(key);
    return value;
}

#undef DEBUG
#endif // DATA_STRUCTURES_LRU_CACHE_HPP
