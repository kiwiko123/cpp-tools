#ifndef DATA_STRUCTURES_LINKED_HASH_MAP_HPP
#define DATA_STRUCTURES_LINKED_HASH_MAP_HPP

// This templated header file defines a LinkedHashMap implementation - 
// a map, which retains the time complexity properties of a standard hash table,
// while also maintaining the order in which objects are added.
// 
// This implementation uses a hash table (std::unordered_map) as its underlying data structure.
// Each item contains a pointer to the previous and next items in the set, conceptually representing a doubly-linked list.
// Bidirectional iterators are implemented simply by following each link to its next/previous neighbor.
//
// The comment-descriptions nested within the member function declarations go into further detail regarding time complexity.
// 
// Author: Geoffrey Ko (2017)
// Developed using the following compiler configuration:
//   compiler: g++ (GCC) 5.4.0
//   flags: -std=c++14 -ggdb

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <cstddef>


template <typename Key,
          typename T, 
          typename Hash = std::hash<Key>, 
          typename Predicate = std::equal_to<Key>>
class LinkedHashMap
{
private:
	typedef std::pair<const Key, T> PairType;
	typedef std::shared_ptr<Key> LinkType;
	struct LinkEntry
	{
		LinkType previous;
		LinkType next;
	};

	struct ValueEntry
	{
		T value;
		LinkEntry link;
	};

public:
	// Constructors
	LinkedHashMap();

	// Operators
	template <typename KeyT, typename TT, typename HashT, typename PredicateT>
	friend std::ostream& operator<<(std::ostream& os, const LinkedHashMap<KeyT, TT, HashT, PredicateT>& obj);

	/* Returns true if the map is NOT empty, or false if it is.
	 * Complexity:
	 *   Θ(1)
	 */
	operator bool() const;

	/* Returns true if both sets have the same values and same ordering.
	 * Complexity: 
	 *   Average case: O(n)
	 *   Best case: Ω(1) when sizes are different
	 */
	bool operator==(const LinkedHashMap<Key, T, Hash, Predicate>& other) const;
	bool operator!=(const LinkedHashMap<Key, T, Hash, Predicate>& other) const;

	/* Returns a reference to the value associated with key.
	 * Can be used to insert {key: value} pairs into the map
	 *   e.g., map[key] = value;
	 * Complexity:
	 *   Best/Average case: O(1)
	 *   Worst case: O(n)
	 */
	T& operator[](const Key& key);

	/* Returns a const-reference to the value associated with key.
	 * Throws std::runtime_erorr if key doesn't exist.
	 * Complexity:
	 *   Best/Average case: O(1)
	 *   Worst case: O(n)
	 */
	const T& operator[](const Key& key) const;

	// Member Functions
	/* Returns the number of items in the map.
	 * Complexity:
	 *   Θ(1)
	 */
	int size() const;

	/* Returns true if key is in the map.
	 * Complexity:
	 *   Best/Average case: Θ(1)
	 *   Worst case: O(n)
	 */
	bool contains(const Key& key) const;

	/* Returns true if no items are in the map (i.e., size() == 0)
	 * Complexity:
	 *   Average case: O(1);
	 *     i.e., Θ(k) where k is the number of objects in the bucket that 'item' hashes to, where k is close to 1.
	 *   Worst (but improbable) case: O(n) if every item in the map hashed into the same bucket.
	 */
	bool empty() const;

	/* Returns a const-reference to the first item in the map.
	 * Complexity:
	 *   Θ(1)
	 */
	const Key& front() const;

	/* Returns a const-reference to the last item in the map.
	 * Complexity:
	 *   Θ(1)
	 */
	const Key& back() const;

	/* Adds {key: value} into the map, "appending" it to the back (in terms of ordering).
	 * Does nothing if item is already contained in the map.
	 * Complexity:
	 *   Average case: amortized O(1)
	 *   Worst case: O(n) when a rehash is triggered.
	 */
	void insert(const Key& key, const T& value);

	/* Removes item from the map, if it exists.
	 * If item is not in the map, throws std::runtime_error.
	 * Complexity:
	 *   Average case: O(1)
	 *   Worst (and improbable) case: O(n) if every item in the map hashed into the same bucket.
	 */
	void erase(const Key& key);

	/* Removes and returns (a copy of) the first {key: value} pair in the map.
	 * Complexity:
	 *   (see LinkedHashMap::erase)
	 */
	PairType pop_front();

	/* Removes and returns (a copy of) the last {key: value} pair in the map.
	 * Complexity:
	 *   (see LinkedHashMap::erase)
	 */
	PairType pop_back();

	/* Removes all items from the map.
	 * Complexity:
	 *   Θ(N)
	 */
	void clear();


private:
	std::unordered_map<Key, ValueEntry, Hash, Predicate> table;
	LinkType head;
	LinkType last;

	// Iterator Implementation
	// By templating the iterator_type class with typename UnqualifiedT,
	// code duplication is virtually eliminated between implementing regular- and const- iterators;
	// adding 2 public typedefs alias iterator_type<T> as 'iterator', and iterator_type<const T> as 'const_iterator'.
	template <typename UnqualifiedT = typename std::remove_const<T>::type>
	class iterator_type : public std::iterator<std::bidirectional_iterator_tag, UnqualifiedT, std::ptrdiff_t, T*, T&>
	{
	public:
		iterator_type(const LinkedHashMap<Key, T, Hash, Predicate>* lmap, LinkType start, unsigned int visited);

		/* Complexity:
		 *   All iterator operations incur O(1) time.
		 */
		bool operator==(const iterator_type& other) const;
		bool operator!=(const iterator_type& other) const;
		auto operator++() -> iterator_type&;
		auto operator++(int) -> iterator_type;
		auto operator--() -> iterator_type&;
		auto operator--(int) -> iterator_type;
		UnqualifiedT& operator*();
		const LinkType operator->() const;

	private:
		const LinkedHashMap<Key, T, Hash, Predicate>* ref;	// raw pointer (instead of smart pointer) because no dynamic allocation or modifying operations are used
		LinkType current;
		unsigned int traversed;	// store a value representing the current "index"
								// to allow Θ(1) iterator equality comparisons.

		/* If the iterator is already past the end, throws std::runtime_error{message}.
		 * Helper member function for operator* and operator->.
		 */
		void bound_check(const std::string& message) const;
	};

	
public:
	typedef iterator_type<const T> const_iterator;
	typedef const_iterator iterator;

	auto cbegin() const -> const_iterator;
	auto cend() const -> const_iterator;
	
	// begin() and end() both return const_iterators;
	// LinkedHashMap does not support non-const iterators, as hash tables cannot have values reassigned (without rehashing them).
	auto begin() const -> iterator;
	auto end() const -> iterator;
};


template <typename Key, typename T, typename Hash, typename Predicate>
LinkedHashMap<Key, T, Hash, Predicate>::LinkedHashMap()
	: head{nullptr}, last{nullptr}
{
}

template <typename Key, typename T, typename Hash, typename Predicate>
std::ostream& operator<<(std::ostream& os, const LinkedHashMap<Key, T, Hash, Predicate>& obj)
{
	os << "LinkedHashMap(";
	auto i = obj.begin();
	if (obj)
	{
		os << *i << ": " << obj[*i];
		++i;
		while (i != obj.end())
		{
			os << ", " << *i << ": " << obj[*i];
			++i;
		}
	}
	os << ")";
	return os;
}

template <typename Key, typename T, typename Hash, typename Predicate>
LinkedHashMap<Key, T, Hash, Predicate>::operator bool() const
{
	return !empty();
}

template <typename Key, typename T, typename Hash, typename Predicate>
bool LinkedHashMap<Key, T, Hash, Predicate>::operator==(const LinkedHashMap<Key, T, Hash, Predicate>& other) const
{
	return table == other.table;
}

template <typename Key, typename T, typename Hash, typename Predicate>
bool LinkedHashMap<Key, T, Hash, Predicate>::operator!=(const LinkedHashMap<Key, T, Hash, Predicate>& other) const
{
	return !operator==(other);
}

template <typename Key, typename T, typename Hash, typename Predicate>
T& LinkedHashMap<Key, T, Hash, Predicate>::operator[](const Key& key)
{
	if (!contains(key))
	{
		insert(key, T{});
	}
	return table[key].value;
}

template <typename Key, typename T, typename Hash, typename Predicate>
const T& LinkedHashMap<Key, T, Hash, Predicate>::operator[](const Key& key) const
{
	if (!contains(key))
	{
		throw std::runtime_error{"key does not exist"};
	}
	return table.at(key).value;
}

template <typename Key, typename T, typename Hash, typename Predicate>
int LinkedHashMap<Key, T, Hash, Predicate>::size() const
{
	return table.size();
}

template <typename Key, typename T, typename Hash, typename Predicate>
bool LinkedHashMap<Key, T, Hash, Predicate>::empty() const
{
	return size() == 0;
}

template <typename Key, typename T, typename Hash, typename Predicate>
bool LinkedHashMap<Key, T, Hash, Predicate>::contains(const Key& key) const
{
	return table.find(key) != table.end();
}

template <typename Key, typename T, typename Hash, typename Predicate>
void LinkedHashMap<Key, T, Hash, Predicate>::insert(const Key& key, const T& value)
{
	if (!contains(key))
	{
		LinkType link{new Key{key}};
		if (!head)
		{
			head = link;
		}
		if (last)
		{
			table[*last].link.next = link;	// link the most-recently inserted item to the about-to-be inserted item
		}
		LinkEntry link_entry;
		link_entry.previous = last;
		link_entry.next = LinkType{nullptr};

		ValueEntry value_entry;
		value_entry.value = value;
		value_entry.link = link_entry;

		table[*link] = value_entry;
		last = link;
	}
}

template <typename Key, typename T, typename Hash, typename Predicate>
void LinkedHashMap<Key, T, Hash, Predicate>::erase(const Key& key)
{
	if (!contains(key))
	{
		throw std::runtime_error{"key does not exist"};
	}
	ValueEntry entry = table.at(key);
	LinkEntry link = entry.link;
	auto previous = link.previous;
	auto next = link.next;
	table.erase(key);

	if (previous)	// if there was a node before 'key', then fix the broken link
	{
		table[*previous].link.next = next;
	}
	else			// otherwise, the head (first key) was just deleted
	{
		head = next;
	}
	if (next)		// if there was a node after 'key', then fix that broken link
	{
		table[*next].link.previous = previous;
	}
	else			// otherwise, the tail (last key) was just deleted
	{
		last = previous;
	}
}

template <typename Key, typename T, typename Hash, typename Predicate>
typename LinkedHashMap<Key, T, Hash, Predicate>::PairType LinkedHashMap<Key, T, Hash, Predicate>::pop_front()
{
	Key key = *head;
	PairType result{key, operator[](key)};
	erase(key);
	return result;
}

template <typename Key, typename T, typename Hash, typename Predicate>
typename LinkedHashMap<Key, T, Hash, Predicate>::PairType LinkedHashMap<Key, T, Hash, Predicate>::pop_back()
{
	Key key = *last;
	PairType result{key, operator[](key)};
	erase(key);
	return result;
}

template <typename Key, typename T, typename Hash, typename Predicate>
void LinkedHashMap<Key, T, Hash, Predicate>::clear()
{
	if (!empty())
	{
		head.reset();
		last.reset();
		table.clear();
	}
}

template <typename Key, typename T, typename Hash, typename Predicate>
auto LinkedHashMap<Key, T, Hash, Predicate>::cbegin() const -> const_iterator
{
	return const_iterator{this, head, 0};
}

template <typename Key, typename T, typename Hash, typename Predicate>
auto LinkedHashMap<Key, T, Hash, Predicate>::cend() const -> const_iterator
{
	unsigned int sz = size();		// silence g++ warning about conflicting types (unsigned int vs. int)
	return const_iterator{this, LinkType{nullptr}, sz};
}

template <typename Key, typename T, typename Hash, typename Predicate>
auto LinkedHashMap<Key, T, Hash, Predicate>::begin() const -> iterator
{
	return cbegin();
}

template <typename Key, typename T, typename Hash, typename Predicate>
auto LinkedHashMap<Key, T, Hash, Predicate>::end() const -> iterator
{
	return cend();
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::iterator_type(const LinkedHashMap<Key, T, Hash, Predicate>* lmap, LinkType start, unsigned int visited)
	: ref{lmap}, current{start}, traversed{visited}
{
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
bool LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator==(const iterator_type<UnqualifiedT>& other) const
{
	return ref == other.ref && traversed == other.traversed;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
bool LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator!=(const iterator_type<UnqualifiedT>& other) const
{
	return !operator==(other);
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator++() -> iterator_type<UnqualifiedT>&
{
	if (current && traversed < ref->size())
	{
		current = ref->table.at(*current).link.next;
		++traversed;
	}
	return *this;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator++(int) -> iterator_type<UnqualifiedT>
{
	iterator_type state{*this};		// invoke implicit copy constructor
	operator++();
	return state;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator--() -> iterator_type<UnqualifiedT>&
{
	if (!current)	// past end
	{
		current = ref->last;
	}
	if (current && traversed >= 0)
	{
		current = ref->table.at(*current).link.previous;
		--traversed;
	}
	return *this;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator--(int) -> iterator_type<UnqualifiedT>
{
	iterator_type state{*this};
	operator--();
	return state;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
UnqualifiedT& LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator*()
{
	bound_check("LinkedHashMap::iterator_type::operator* - iterator past end");
	return *current;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
const typename LinkedHashMap<Key, T, Hash, Predicate>::LinkType LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator->() const
{
	bound_check("LinkedHashMap::iterator_type::operator-> iterator past end");
	return current;
}

template <typename Key, typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
void LinkedHashMap<Key, T, Hash, Predicate>::iterator_type<UnqualifiedT>::bound_check(const std::string& message) const
{
	if (!current)
	{
		throw std::runtime_error{message};
	}
}

#endif // DATA_STRUCTURES_LINKED_HASH_MAP_HPP
