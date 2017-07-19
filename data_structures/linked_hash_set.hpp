#ifndef _LINKED_HASH_SET_HPP
#define _LINKED_HASH_SET_HPP

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

	template <typename InputIterator>
	LinkedHashSet(InputIterator first, InputIterator last);

	// Operators
	template <typename TT>
	friend std::ostream& operator<<(std::ostream& os, const LinkedHashSet<TT>& obj);

	operator bool() const;
	bool operator==(const LinkedHashSet<T>& other) const;
	bool operator!=(const LinkedHashSet<T>& other) const;

	// Member Functions
	int size() const;
	bool contains(const T& item) const;
	bool empty() const;

	void insert(const T& item);
	void erase(const T& item);
	void clear();


	class iterator;
	auto begin() const -> iterator;
	auto end() const -> iterator;

	class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
	{
	public:
		iterator(LinkedHashSet<T>* set, LinkType start, unsigned int visited);
		iterator(LinkedHashSet<T>* set, LinkType start);

		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;
		auto operator++() -> iterator&;
		auto operator++(int) -> iterator;
		auto operator--() -> iterator&;
		auto operator--(int) -> iterator;
		const T& operator*() const;
		const T* operator->() const;

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

		table[item] = std::move(entry);
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
void LinkedHashSet<T>::iterator::bound_check(const std::string& message) const
{
	if (!current)
	{
		throw std::runtime_error{message};
	}
}

#endif // _LINKED_HASH_SET_HPP
