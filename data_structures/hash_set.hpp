// A set is a container that stores unique items.
// Sets can be efficiently implemented using a hash table as its data structure.
// Hash tables allow for O(1) operations for insertion, deletion, and lookup.
//
// Iterators are also space-efficiently implemented by keeping track of 3 simple items:
//   * a pointer to the set being iterated over
//   * a single LinkedList::iterator cursor representing the current position
//   * a count of the number of items already traversed
// But, performance is not optimal because advancement relies on exception handling.
#ifndef HASH_SET_HPP
#define HASH_SET_HPP

#include <iostream>
#include <iterator>
#include <functional>
#include <stdexcept>
#include <sstream>
#include <string>
#include <cstdlib>
#include "linked_list.hpp"


namespace
{
	double _HASH_SET_LOAD_FACTOR_THRESHOLD = 1.0;
	int _HASH_SET_INITIAL_SIZE = 5;
}



template <typename T>
class HashSet
{
public:
	HashSet(const std::function<int(const T&)>& hasher, double the_load_factor);
	explicit HashSet(const std::function<int(const T&)>& hasher);
	HashSet(const HashSet& right);

	template <typename Container>
	HashSet(const Container& iterable, const std::function<int(const T&)>& hasher);

	~HashSet();


	// Operators
	HashSet<T>& operator=(const HashSet<T>& right);

	/* Two hash_sets are == if they have the same elements within them.
	 * hash_sets do not have to have the same order, or same hash function to be ==.
	 */
	bool operator==(const HashSet<T>& right) const;
	bool operator!=(const HashSet<T>& right) const;
	bool operator<(const HashSet<T>& other) const;
	bool operator<=(const HashSet<T>& other) const;
	bool operator>(const HashSet<T>& other) const;
	bool operator>=(const HashSet<T>& other) const;

	/* Returns a new hash_set containing all items in both this and right;
	 * non-modifying version of hash_set::combine
	 */
	HashSet<T> operator+(const HashSet<T>& right) const;

	/* Returns a new hash_set containing all elements in this that are NOT in right;
	 * e.g., non-modifying version of hash_set::difference
	 */
	HashSet<T> operator-(const HashSet<T>& right) const;

	template <typename T2>
	friend std::ostream& operator<<(std::ostream& os, const HashSet<T2>& hm);

	// Non-Modifying Member Functions
	int size() const;
	bool empty() const;
	bool contains(const T& item) const;
	std::string str() const;


	// Modifying Member Functions
	/* Inserts item into the set.
	 * If item is already in the set, replaces the existing one with parameter item.
	 */
	void insert(const T& item);

	/* Removes item from the map.
	 * Throws std::invalid_argument if item is not in the set
	 */
	void erase(const T& item);

	/* Removes all items in this that are NOT in other */
	void difference(const HashSet<T>& other);

	/* Combines this with other; duplicates are not overwritten */
	void combine(const HashSet<T>& other);




	class iterator;
	auto begin() const -> iterator;
	auto end() const -> iterator;

	class iterator : public std::iterator<std::forward_iterator_tag, T>
	{
	public:
		iterator();

		auto operator++() -> iterator&;
		auto operator++(int) -> iterator;
		bool operator==(const iterator& right) const;
		bool operator!=(const iterator& right) const;
		T& operator*() const;
		T* operator->() const;

		friend iterator HashSet<T>::begin() const;
		friend iterator HashSet<T>::end() const;

	private:
		iterator(HashSet<T>* it, int already);

		bool done() const;
		void advance_bin();
		void next();

		HashSet<T>* ref;
		int traversed;
		typename LinkedList<T>::iterator current;
		int current_bin_index;
	};




protected:
	std::function<int(const T&)> hash;
	int bins;
	LinkedList<T>* table;


private:
	double lft;
	unsigned int length;

	void _rehash();
	double load_factor() const;
	int get_bin(const T& item) const;
};


template <typename T>
HashSet<T>::HashSet(const std::function<int(const T&)>& hasher, double the_load_factor)
	: hash{hasher}, bins{_HASH_SET_INITIAL_SIZE}, table{new LinkedList<T>[bins]}, lft{the_load_factor}, length{0}
{
}

template <typename T>
HashSet<T>::HashSet(const std::function<int(const T&)>& hasher) : HashSet{hasher, _HASH_SET_LOAD_FACTOR_THRESHOLD}
{
}

template <typename T>
template <typename Container>
HashSet<T>::HashSet(const Container& iterable, const std::function<int(const T&)>& hasher) : HashSet{hasher}
{
	for (const auto& item : iterable)
		insert(item);
}

template <typename T>
HashSet<T>::HashSet(const HashSet<T>& right)
	: hash{right.hash}, bins{right.bins}, table{new LinkedList<T>[bins]}, lft{right.lft}, length{right.length}
{
	for (unsigned int i = 0; i < right.bins; ++i)
		table[i] = right.table[i];
}

template <typename T>
HashSet<T>::~HashSet()
{
	delete[] table;
}

template <typename T>
HashSet<T>& HashSet<T>::operator=(const HashSet<T>& right)
{
	if (this != &right)
	{
		delete[] table;
		hash = right.hash;
		bins = right.bins;
		lft = right.lft;
		length = right.length;
		table = new LinkedList<T>[bins];

		for (unsigned int i = 0; i < bins; ++i)
			table[i] = right.table[i];
	}
	return *this;
}

template <typename T>
bool HashSet<T>::operator==(const HashSet<T>& right) const
{
	if (this == &right)
		return true;
	if (size() != right.size())
		return false;

	for (unsigned int i = 0; i < right.bins; ++i)
	{
		for (const auto& item : right.table[i])
		{
			if (!contains(item))
				return false;
		}
	}
	return true;
}

template <typename T>
bool HashSet<T>::operator!=(const HashSet<T>& right) const
{
	return !operator==(right);
}

template <typename T>
bool HashSet<T>::operator<(const HashSet<T>& other) const
{
	return operator<=(other) && operator!=(other);
}

template <typename T>
bool HashSet<T>::operator<=(const HashSet<T>& other) const
{
	if (size() > other.size())
		return false;

	for (unsigned int i = 0; i < bins; ++i)
	{
		for (const auto& item : table[i])
		{
			if (!other.contains(item))
				return false;
		}
	}
	return true;
}

template <typename T>
bool HashSet<T>::operator>(const HashSet<T>& other) const
{
	return other < *this;
}

template <typename T>
bool HashSet<T>::operator>=(const HashSet<T>& other) const
{
	return !operator<(other);
}

template <typename T>
HashSet<T> HashSet<T>::operator+(const HashSet<T>& right) const
{
	HashSet<T> result{*this};
	result.combine(right);

	return result;
}

template <typename T>
HashSet<T> HashSet<T>::operator-(const HashSet<T>& right) const
{
	HashSet<T> result{hash};
	for (unsigned int i = 0; i < bins; ++i)
	{
		for (const auto& item : table[i])
		{
			if (!right.contains(item))
				result.insert(item);
		}
	}
	return result;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const HashSet<T>& set)
{
	os << "hash_set(";
	unsigned int c = 0;
	for (unsigned int i = 0; i < set.bins; ++i)
	{
		for (const auto& item : set.table[i])
			os << item << (c++ < set.size() - 1 ? ", " : "");
	}
	os << ")";

	return os;
}

template <typename T>
int HashSet<T>::size() const
{
	return length;
}

template <typename T>
bool HashSet<T>::empty() const
{
	return size() == 0;
}

template <typename T>
bool HashSet<T>::contains(const T& item) const
{
	return table[get_bin(item)].contains(item);
}

template <typename T>
std::string HashSet<T>::str() const
{
	std::ostringstream result;
	result << "hash_set(" << std::endl;
	for (unsigned int i = 0; i < bins; ++i)
		result << "  " << i << ": " << table[i] << std::endl;

	result << ")";
	return result.str();
}

template <typename T>
void HashSet<T>::insert(const T& item)
{
	if (contains(item))
		erase(item);
	if (load_factor() >= lft)
		_rehash();

	table[get_bin(item)].push_front(item);
	++length;
}

template <typename T>
void HashSet<T>::erase(const T& item)
{
	if (!contains(item))
		throw std::invalid_argument{"key is not in map"};

	table[get_bin(item)].erase(item);
	--length;
}

template <typename T>
void HashSet<T>::difference(const HashSet<T>& other)
{
	LinkedList<T> to_remove;
	for (unsigned int i = 0; i < bins; ++i)
	{
		for (const auto& item : table[i])
		{
			if (other.contains(item))
				to_remove.push_front(item);
		}
	}
	for (const auto& item : to_remove)
		erase(item);
}

template <typename T>
void HashSet<T>::combine(const HashSet<T>& other)
{
	for (unsigned int i = 0; i < other.bins; ++i)
	{
		for (const auto& item : other.table[i])
		{
			if (!contains(item))
				insert(item);
		}
	}
}

template <typename T>
double HashSet<T>::load_factor() const
{
	return size() / static_cast<double>(bins);
}

template <typename T>
void HashSet<T>::_rehash()
{
	int old_bins = bins;
	bins *= 2;
	LinkedList<T>* reallocated = new LinkedList<T>[bins];
	for (unsigned int i = 0; i < old_bins; ++i)
	{
		for (const auto& item : table[i])
			reallocated[get_bin(item)].push_back(item);
	}

	delete[] table;
	table = reallocated;
}

template <typename T>
int HashSet<T>::get_bin(const T& item) const
{
	return std::abs(hash(item)) % bins;
}


// iterator implementation
template <typename T>
auto HashSet<T>::begin() const -> HashSet<T>::iterator
{
	return iterator{const_cast<HashSet<T>*>(this), 0};
}

template <typename T>
auto HashSet<T>::end() const -> HashSet<T>::iterator
{
	return iterator{const_cast<HashSet<T>*>(this), size()};
}

template <typename T>
HashSet<T>::iterator::iterator(HashSet<T>* it, int already)
	: ref{it}, traversed{already}, current_bin_index{-1}
{
	advance_bin();
}

template <typename T>
HashSet<T>::iterator::iterator() : iterator{nullptr, 0}
{
}

template <typename T>
bool HashSet<T>::iterator::done() const
{
	return traversed >= ref->size();
}

template <typename T>
void HashSet<T>::iterator::advance_bin()
{
	if (current_bin_index >= ref->bins)
		throw std::out_of_range{"hash_set::iterator::advance_bin() -- cursor past end"};

	for (++current_bin_index; current_bin_index < ref->bins && ref->table[current_bin_index].empty(); ++current_bin_index)
	{ /* advance current_bin_index to next non-empty bin */ }
	current = ref->table[current_bin_index].begin();
}

template <typename T>
void HashSet<T>::iterator::next()
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

template <typename T>
auto HashSet<T>::iterator::operator++() -> HashSet<T>::iterator&
{
	if (!done())
		next();

	return *this;
}

template <typename T>
auto HashSet<T>::iterator::operator++(int) -> HashSet<T>::iterator
{
	iterator to_return{*this};
	operator++();

	return to_return;
}

template <typename T>
bool HashSet<T>::iterator::operator==(const HashSet<T>::iterator& right) const
{
	return ref == right.ref && traversed == right.traversed;
}

template <typename T>
bool HashSet<T>::iterator::operator!=(const HashSet<T>::iterator& right) const
{
	return !operator==(right);
}

template <typename T>
T& HashSet<T>::iterator::operator*() const
{
	if (done())
		throw std::out_of_range{"hash_set::iterator::operator* -- cursor past end"};

	return *current;
}

template <typename T>
T* HashSet<T>::iterator::operator->() const
{
	if (done())
		throw std::out_of_range{"hash_set::iterator::operator-> -- cursor past end"};

	return current;
}


#endif // HASH_SET_HPP
