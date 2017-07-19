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

#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>


template <typename T>
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
	template <typename TT>
	friend std::ostream& operator<<(std::ostream& os, const LinkedHashSet<TT>& obj);

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
	bool operator==(const LinkedHashSet<T>& other) const;
	bool operator!=(const LinkedHashSet<T>& other) const;

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
	 *   Θ(1)
	 */
	bool empty() const;

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
	 *   Worst (and improbable) case: O(N) if every item in the set were to collide into the same bucket.
	 */
	void erase(const T& item);

	/* Removes all items from the set.
	 * Complexity:
	 *   Θ(N)
	 */
	void clear();


	class iterator;
	auto begin() const -> iterator;
	auto end() const -> iterator;

	class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
	{
	public:
		iterator(LinkedHashSet<T>* set, LinkType start, unsigned int visited);

		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;
		auto operator++() -> iterator&;
		auto operator++(int) -> iterator;
		auto operator--() -> iterator&;
		auto operator--(int) -> iterator;
		const T& operator*() const;
		const LinkType operator->() const;

	private:
		LinkedHashSet<T>* ref;
		LinkType current;
		unsigned int traversed;

		void bound_check(const std::string& message) const;
	};


private:
	std::unordered_map<T, LinkEntry> table;
	LinkType head;
	LinkType last;
};


template <typename T>
LinkedHashSet<T>::LinkedHashSet()
	: head{nullptr}, last{nullptr}
{
}

template <typename T>
template <typename InputIterator>
LinkedHashSet<T>::LinkedHashSet(InputIterator first, InputIterator last) : LinkedHashSet<T>{}
{
	while (first != last)
	{
		insert(*(first++));
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const LinkedHashSet<T>& obj)
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

template <typename T>
LinkedHashSet<T>::operator bool() const
{
	return !empty();
}

template <typename T>
bool LinkedHashSet<T>::operator==(const LinkedHashSet<T>& other) const
{
	return size() == other.size();
}

template <typename T>
bool LinkedHashSet<T>::operator!=(const LinkedHashSet<T>& other) const
{
	return !operator==(other);
}

template <typename T>
int LinkedHashSet<T>::size() const
{
	return table.size();
}

template <typename T>
bool LinkedHashSet<T>::empty() const
{
	return size() == 0;
}

template <typename T>
bool LinkedHashSet<T>::contains(const T& item) const
{
	return table.find(item) != table.end();
}

template <typename T>
void LinkedHashSet<T>::insert(const T& item)
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

template <typename T>
void LinkedHashSet<T>::erase(const T& item)
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

template <typename T>
void LinkedHashSet<T>::clear()
{
	if (!empty())
	{
		table.clear();
		head.reset();
		last.reset();
	}
}

template <typename T>
auto LinkedHashSet<T>::begin() const -> iterator
{
	return iterator{const_cast<LinkedHashSet<T>*>(this), head, 0};
}

template <typename T>
auto LinkedHashSet<T>::end() const -> iterator
{
	unsigned int sz = size();		// silence g++ warning about conflicting types (unsigned int vs. int)
	return iterator{const_cast<LinkedHashSet<T>*>(this), LinkType{nullptr}, sz};
}

template <typename T>
LinkedHashSet<T>::iterator::iterator(LinkedHashSet<T>* set, LinkType start, unsigned int visited)
	: ref{set}, current{start}, traversed{visited}
{
}

template <typename T>
bool LinkedHashSet<T>::iterator::operator==(const iterator& other) const
{
	return ref == other.ref && traversed == other.traversed;
}

template <typename T>
bool LinkedHashSet<T>::iterator::operator!=(const iterator& other) const
{
	return !operator==(other);
}

template <typename T>
auto LinkedHashSet<T>::iterator::operator++() -> iterator&
{
	if (current && traversed < ref->size())
	{
		current = ref->table.at(*current).next;
		++traversed;
	}
	return *this;
}

template <typename T>
auto LinkedHashSet<T>::iterator::operator++(int) -> iterator
{
	iterator state{*this};		// invoke implicit copy constructor
	operator++();
	return state;
}

template <typename T>
auto LinkedHashSet<T>::iterator::operator--() -> iterator&
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

template <typename T>
auto LinkedHashSet<T>::iterator::operator--(int) -> iterator
{
	iterator state{*this};
	operator--();
	return state;
}

template <typename T>
const T& LinkedHashSet<T>::iterator::operator*() const
{
	bound_check("LinkedHashSet::iterator::operator* - iterator past end");
	return *current;
}

template <typename T>
const typename LinkedHashSet<T>::LinkType LinkedHashSet<T>::iterator::operator->() const
{
	bound_check("LinkedHashSet::iterator::operator-> iterator past end");
	return current;
}

template <typename T>
void LinkedHashSet<T>::iterator::bound_check(const std::string& message) const
{
	if (!current)
	{
		throw std::runtime_error{message};
	}
}

#endif // DATA_STRUCTURES_LINKED_HASH_SET_HPP
