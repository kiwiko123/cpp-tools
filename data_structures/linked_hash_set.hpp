#ifndef _LINKED_HASH_SET_HPP
#define _LINKED_HASH_SET_HPP

#include <iostream>
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
	LinkedHashSet();

	template <typename TT>
	friend std::ostream& operator<<(std::ostream& os, const LinkedHashSet<TT>& obj);

	bool operator==(const LinkedHashSet<T>& other) const;
	bool operator!=(const LinkedHashSet<T>& other) const;

	int size() const;
	bool contains(const T& item) const;
	bool empty() const;

	void insert(const T& item);
	void erase(const T& item);
	void clear();

	void print() const;


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
std::ostream& operator<<(std::ostream& os, const LinkedHashSet<T>& obj)
{
	os << "LinkedHashSet(";
	os << ")";
	return os;
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
void LinkedHashSet<T>::print() const
{
	auto current = head;
	while (current)
	{
		std::cout << *current << std::endl;
		current = table.at(*current).next;
	}
}

#endif // _LINKED_HASH_SET_HPP
