#ifndef DATA_STRUCTURES_LINKED_HASH_SET_HPP
#define DATA_STRUCTURES_LINKED_HASH_SET_HPP

// This templated header file defines a LinkedHashSet implementation - 
// a set, which retains the time complexity properties of a standard hash table,
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


template <typename T, 
          typename Hash = std::hash<T>, 
          typename Predicate = std::equal_to<T>>
class LinkedHashSet
{
private:
	typedef std::shared_ptr<T> LinkType;
	struct LinkEntry
	{
		LinkType previous;
		LinkType next;
	};

public:
	// Constructors
	LinkedHashSet();

	/* Range constructor - populates set with elements [first, last).
	 * Maintains the order produced by InputIterator::operator++().
	 */
	template <typename InputIterator>
	LinkedHashSet(InputIterator first, InputIterator last);

	// Operators
	template <typename TT, typename HashT, typename PredicateT>
	friend std::ostream& operator<<(std::ostream& os, const LinkedHashSet<TT, HashT, PredicateT>& obj);

	/* Returns true if the set is NOT empty, or false if it is.
	 * Complexity:
	 *   Θ(1)
	 */
	operator bool() const;

	/* Returns true if both sets have the same values and same ordering.
	 * Complexity: 
	 *   Average case: O(N)
	 *   Best case: Ω(1) when sizes are different
	 */
	bool operator==(const LinkedHashSet<T, Hash, Predicate>& other) const;
	bool operator!=(const LinkedHashSet<T, Hash, Predicate>& other) const;

	/* The complexity of each of the following relational operators is Θ(min(N, K)), 
	 * where K is the number of elements in 'other'
	 */

	/* Returns true if this is a proper subset of other */
	bool operator<(const LinkedHashSet<T, Hash, Predicate>& other) const;

	/* Returns true if this is a subset of other */
	bool operator<=(const LinkedHashSet<T, Hash, Predicate>& other) const;

	/* Returns true if this is a proper superset of other */
	bool operator>(const LinkedHashSet<T, Hash, Predicate>& other) const;

	/* Returns true if this is a superset of other */
	bool operator>=(const LinkedHashSet<T, Hash, Predicate>& other) const;



	// Member Functions
	/* Returns the number of items in the set.
	 * Complexity:
	 *   Θ(1)
	 */
	int size() const;

	/* Returns true if item is in the set.
	 * Complexity:
	 *   Θ(1)
	 */
	bool contains(const T& item) const;

	/* Returns true if no items are in the set (i.e., size() == 0)
	 * Complexity:
	 *   Average case: O(1); Θ(k) where k is the number of objects in the bucket that 'item' hashes to.
	 *   Worst (but improbable) case: O(N) if every item in the set hashed into the same bucket.
	 */
	bool empty() const;

	/* Returns a const-reference to the first item in the set.
	 * Complexity:
	 *   Θ(1)
	 */
	const T& front() const;

	/* Returns a const-reference to the last item in the set.
	 * Complexity:
	 *   Θ(1)
	 */
	const T& back() const;

	/* Adds item into the set, "appending" it to the back (in terms of ordering).
	 * Does nothing if item is already contained in the set.
	 * Complexity:
	 *   Average case: amortized O(1)
	 *   Worst case: O(N) when a rehash is triggered.
	 */
	void insert(const T& item);

	/* Removes item from the set, if it exists.
	 * If item is not in the set, throws std::runtime_error.
	 * Complexity:
	 *   Average case: O(1)
	 *   Worst (and improbable) case: O(N) if every item in the set hashed into the same bucket.
	 */
	void erase(const T& item);

	/* Removes and returns (a copy of) the first item in the set.
	 * Complexity:
	 *   (see LinkedHashSet::erase)
	 */
	T pop_front();

	/* Removes and returns (a copy of) the last item in the set.
	 * Complexity:
	 *   (see LinkedHashSet::erase)
	 */
	T pop_back();

	/* Removes all items from the set.
	 * Complexity:
	 *   Θ(N)
	 */
	void clear();


private:
	std::unordered_map<T, LinkEntry, Hash, Predicate> table;
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
		iterator_type(const LinkedHashSet<T, Hash, Predicate>* set, LinkType start, unsigned int visited);

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
		const LinkedHashSet<T, Hash, Predicate>* ref;	// raw pointer (instead of smart pointer) because no dynamic allocation or modifying operations are used
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
	// LinkedHashSet does not support non-const iterators, as hash tables cannot have values reassigned (without rehashing them).
	auto begin() const -> iterator;
	auto end() const -> iterator;
};


template <typename T, typename Hash, typename Predicate>
LinkedHashSet<T, Hash, Predicate>::LinkedHashSet()
	: head{nullptr}, last{nullptr}
{
}

template <typename T, typename Hash, typename Predicate>
template <typename InputIterator>
LinkedHashSet<T, Hash, Predicate>::LinkedHashSet(InputIterator first, InputIterator last) : LinkedHashSet<T, Hash, Predicate>{}
{
	while (first != last)
	{
		insert(*(first++));
	}
}

template <typename T, typename Hash, typename Predicate>
std::ostream& operator<<(std::ostream& os, const LinkedHashSet<T, Hash, Predicate>& obj)
{
	os << "LinkedHashSet(";
	auto i = obj.begin();
	if (obj)
	{
		os << *(i++);
		while (i != obj.end())
		{
			os << ", " << *(i++);
		}
	}
	os << ")";
	return os;
}

template <typename T, typename Hash, typename Predicate>
LinkedHashSet<T, Hash, Predicate>::operator bool() const
{
	return !empty();
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::operator==(const LinkedHashSet<T, Hash, Predicate>& other) const
{
	return table == other.table;
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::operator!=(const LinkedHashSet<T, Hash, Predicate>& other) const
{
	return !operator==(other);
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::operator<(const LinkedHashSet<T, Hash, Predicate>& other) const
{
	return other > *this;
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::operator<=(const LinkedHashSet<T, Hash, Predicate>& other) const
{
	return !(other < *this);
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::operator>(const LinkedHashSet<T, Hash, Predicate>& other) const
{
	return size() > other.size() && *this >= other;
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::operator>=(const LinkedHashSet<T, Hash, Predicate>& other) const
{
	return std::includes(begin(), end(), other.begin(), other.end());
}

template <typename T, typename Hash, typename Predicate>
int LinkedHashSet<T, Hash, Predicate>::size() const
{
	return table.size();
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::empty() const
{
	return size() == 0;
}

template <typename T, typename Hash, typename Predicate>
bool LinkedHashSet<T, Hash, Predicate>::contains(const T& item) const
{
	return table.find(item) != table.end();
}

template <typename T, typename Hash, typename Predicate>
void LinkedHashSet<T, Hash, Predicate>::insert(const T& item)
{
	if (!contains(item))
	{
		LinkType link{new T{item}};
		if (!head)
		{
			head = link;
		}
		if (last)
		{
			table[*last].next = link;	// link the most-recently inserted item to the about-to-be inserted item
		}
		LinkEntry entry;
		entry.previous = last;
		entry.next = LinkType{nullptr};

		table[*link] = std::move(entry);
		last = link;
	}
}

template <typename T, typename Hash, typename Predicate>
void LinkedHashSet<T, Hash, Predicate>::erase(const T& item)
{
	if (!contains(item))
	{
		throw std::runtime_error{"item does not exist"};
	}
	LinkEntry link = table.at(item);
	auto previous = link.previous;
	auto next = link.next;
	table.erase(item);

	if (previous)	// if there was a node before 'item', then fix the broken link
	{
		table[*previous].next = next;
	}
	else			// otherwise, the head (first item) was just deleted
	{
		head = next;
	}
	if (next)		// if there was a node after 'item', then fix that broken link
	{
		table[*next].previous = previous;
	}
	else			// otherwise, the tail (last item) was just deleted
	{
		last = previous;
	}
}

template <typename T, typename Hash, typename Predicate>
T LinkedHashSet<T, Hash, Predicate>::pop_front()
{
	T value = *head;
	erase(value);
	return value;
}

template <typename T, typename Hash, typename Predicate>
T LinkedHashSet<T, Hash, Predicate>::pop_back()
{
	T value = *last;
	erase(value);
	return value;
}

template <typename T, typename Hash, typename Predicate>
void LinkedHashSet<T, Hash, Predicate>::clear()
{
	if (!empty())
	{
		head.reset();
		last.reset();
		table.clear();
	}
}

template <typename T, typename Hash, typename Predicate>
auto LinkedHashSet<T, Hash, Predicate>::cbegin() const -> const_iterator
{
	return const_iterator{this, head, 0};
}

template <typename T, typename Hash, typename Predicate>
auto LinkedHashSet<T, Hash, Predicate>::cend() const -> const_iterator
{
	unsigned int sz = size();		// silence g++ warning about conflicting types (unsigned int vs. int)
	return const_iterator{this, LinkType{nullptr}, sz};
}

template <typename T, typename Hash, typename Predicate>
auto LinkedHashSet<T, Hash, Predicate>::begin() const -> iterator
{
	return cbegin();
}

template <typename T, typename Hash, typename Predicate>
auto LinkedHashSet<T, Hash, Predicate>::end() const -> iterator
{
	return cend();
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::iterator_type(const LinkedHashSet<T, Hash, Predicate>* set, LinkType start, unsigned int visited)
	: ref{set}, current{start}, traversed{visited}
{
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
bool LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator==(const iterator_type<UnqualifiedT>& other) const
{
	return ref == other.ref && traversed == other.traversed;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
bool LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator!=(const iterator_type<UnqualifiedT>& other) const
{
	return !operator==(other);
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator++() -> iterator_type<UnqualifiedT>&
{
	if (current && traversed < ref->size())
	{
		current = ref->table.at(*current).next;
		++traversed;
	}
	return *this;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator++(int) -> iterator_type<UnqualifiedT>
{
	iterator_type state{*this};		// invoke implicit copy constructor
	operator++();
	return state;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator--() -> iterator_type<UnqualifiedT>&
{
	if (!current)	// past end
	{
		current = ref->last;
	}
	if (current && traversed >= 0)
	{
		current = ref->table.at(*current).previous;
		--traversed;
	}
	return *this;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
auto LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator--(int) -> iterator_type<UnqualifiedT>
{
	iterator_type state{*this};
	operator--();
	return state;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
UnqualifiedT& LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator*()
{
	bound_check("LinkedHashSet::iterator_type::operator* - iterator past end");
	return *current;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
const typename LinkedHashSet<T, Hash, Predicate>::LinkType LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::operator->() const
{
	bound_check("LinkedHashSet::iterator_type::operator-> iterator past end");
	return current;
}

template <typename T, typename Hash, typename Predicate>
template <typename UnqualifiedT>
void LinkedHashSet<T, Hash, Predicate>::iterator_type<UnqualifiedT>::bound_check(const std::string& message) const
{
	if (!current)
	{
		throw std::runtime_error{message};
	}
}

#endif // DATA_STRUCTURES_LINKED_HASH_SET_HPP
