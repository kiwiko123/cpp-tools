// This module defines a standard singly-linked list implementation.
// Caches a rear pointer, so appending is done in O(1) time.
#ifndef LINKED_LIST_HPP
#define LINKED_LIST_HPP

#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>



template <typename T>
class LinkedList
{
public:
	LinkedList();
	LinkedList(const LinkedList<T>& right);

	template <typename iterable>
	explicit LinkedList(const iterable& v);

	~LinkedList();

	// --- Operators ---
	// Assigns right into this.
	// Efficiently reuses dynamically allocated memory as necessary.
	LinkedList<T>& operator=(const LinkedList<T>& right);
	bool operator==(const LinkedList<T>& right) const;
	bool operator!=(const LinkedList<T>& right) const;

	template <typename T2>
	friend std::ostream& operator<<(std::ostream& os, const LinkedList<T>& right);


	// --- Member Functions ---
	/* Returns the number of items stored in this */
	int size() const;

	/* Returns true if there are no items stored in this */
	bool empty() const;

	/* Returns true if item is in this, or false if not */
	bool contains(const T& item) const;

	/* Returns a string representation of the linked list */
	std::string str() const;
	
	/* Returns a reference to the first item in the linked list */
	const T& first() const;
	
	/* Returns a reference to the last item in the linked list */
	const T& last() const;

	/* Removes all items from this */
	void clear();

	/* Removes the first occurrence of item in the linked list */
	void erase(const T& item);

	/* Appends item to the back of the linked list.
	 * O(1).
	 */
	void push_back(const T& item);

	/* Adds item to the front of the linked list.
	 * O(1).
	 */
	void push_front(const T& item);

	/* Removes the last item in the linked list.
	 * O(N).
	 */
	T pop_back();

	/* Removes the first item in the linked list.
	 * O(1).
	 */
	T pop_front();


protected:
	struct node
	{
	public:
		T value;
		node* next;
	};
	node* front;
	node* rear;


public:
	class iterator;
	auto begin() const -> iterator;
	auto end() const -> iterator;

	class iterator : public std::iterator<std::forward_iterator_tag, T>
	{
	public:
		auto operator++() -> iterator&;
		auto operator++(int) -> iterator;
		bool operator==(const iterator& right) const;
		bool operator!=(const iterator& right) const;
		T& operator*() const;
		T* operator->() const;

		friend iterator LinkedList<T>::begin() const;
		friend iterator LinkedList<T>::end() const;

		iterator();

	private:
		iterator(LinkedList<T>* ref, node* start);

		LinkedList<T>* ref;
		node* current;
	};


private:
	unsigned int length;

	void deallocate_list(node*& start);
	void _ensure_rear();
};




template <typename T>
LinkedList<T>::LinkedList()
	: front{nullptr}, rear{front}, length{0}
{
}

template <typename T>
LinkedList<T>::LinkedList(const LinkedList<T>& right)
	: front{nullptr}, rear{front}, length{0}
{
	for (node* current = right.front; current != nullptr; current = current->next)
		push_back(current->value);
}

template <typename T>
template <typename iterable>
LinkedList<T>::LinkedList(const iterable& v) : LinkedList<T>{}
{
	for (const auto& i : v)
		push_back(i);
}

template <typename T>
LinkedList<T>::~LinkedList()
{
	deallocate_list(front);
}

template <typename T>
LinkedList<T>& LinkedList<T>::operator=(const LinkedList<T>& right)
{
	if (this != &right)
	{
		while (size() > right.size())
			pop_front();

		node* rc = right.front;
		for (node* c = front; c != nullptr && rc != nullptr; c = c->next, rc = rc->next)
			c->value = rc->value;

		// allocate new nodes for leftovers
		for (; rc != nullptr; rc = rc->next)
			push_back(rc->value);
	}
	return *this;
}

template <typename T>
bool LinkedList<T>::operator==(const LinkedList<T>& right) const
{
	if (this == &right)
		return true;
	if (size() != right.size())
		return false;

	for (node *current = front, *rcurrent = right.front;
		 current != nullptr; // if size() == right.size() then rcurrent will also != nullptr
		 current = current->next, rcurrent = rcurrent->next)
	{
		if (current->value != rcurrent->value)
			return false;
	}
	return true;
}

template <typename T>
bool LinkedList<T>::operator!=(const LinkedList<T>& right) const
{
	return !operator==(right);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const LinkedList<T>& right)
{
	os << right.str();
	return os;
}

template <typename T>
int LinkedList<T>::size() const
{
	return length;
}

template <typename T>
bool LinkedList<T>::empty() const
{
	return size() == 0;
}

template <typename T>
bool LinkedList<T>::contains(const T& item) const
{
	for (node* current = front; current != nullptr; current = current->next)
	{
		if (current->value == item)
			return true;
	}
	return false;
}

template <typename T>
std::string LinkedList<T>::str() const
{
	std::ostringstream result;
	result << "linked_list(";
	if (!empty())
	{
		result << front->value;
		for (node* current = front->next; current != nullptr; current = current->next)
			result << "->" << current->value;
	}
	result << ")";
	return result.str();
}

template <typename T>
void LinkedList<T>::clear()
{
	deallocate_list(front);
	front = rear = nullptr;
	length = 0;
}

template <typename T>
const T& LinkedList<T>::first() const
{
	return front->value;
}

template <typename T>
const T& LinkedList<T>::last() const
{
	return rear->value;
}

template <typename T>
void LinkedList<T>::erase(const T& item)
{
	for (node** c = &front; *c != nullptr; c = &((*c)->next))
	{
		if ((*c)->value == item)
		{
			node* last = *c;
			*c = (*c)->next;
			delete last;

			--length;
			if (*c == nullptr)
				_ensure_rear();
			return;
		}
	}
	throw std::invalid_argument{"LinkedList::erase\n  item not in LinkedList"};
}

template <typename T>
void LinkedList<T>::push_back(const T& item)
{
	if (empty())
		front = rear = new node{item};
	else
		rear = rear->next = new node{item};

	++length;
}

template <typename T>
void LinkedList<T>::push_front(const T& item)
{
	front = new node{item, front};
	if (empty())
		rear = front;

	++length;
}

template <typename T>
T LinkedList<T>::pop_back()
{
	if (empty())
		throw std::out_of_range{"LinkedList::pop_back -- empty"};
	else if (size() == 1)
		return pop_front();

	node* c;
	for (c = front; c->next != rear; c = c->next)
	{
	}

	rear = c;
	T value = c->next->value;
	delete c->next;
	c->next = nullptr;

	--length;
	return value;
}

template <typename T>
T LinkedList<T>::pop_front()
{
	if (empty())
		throw std::out_of_range{"LinkedList::pop_front - empty"};

	T value = front->value;
	node* to_delete = front;
	front = front->next;
	delete to_delete;

	--length;
	return value;
}

template <typename T>
auto LinkedList<T>::begin() const -> LinkedList<T>::iterator
{
	return iterator{const_cast<LinkedList<T>*>(this), front};
}

template <typename T>
auto LinkedList<T>::end() const -> LinkedList<T>::iterator
{
	return iterator{const_cast<LinkedList<T>*>(this), nullptr};
}

template <typename T>
LinkedList<T>::iterator::iterator(LinkedList<T>* ll, node* start)
	: ref{ll}, current{start}
{
}

template <typename T>
LinkedList<T>::iterator::iterator() : iterator(nullptr, nullptr)
{
}

template <typename T>
auto LinkedList<T>::iterator::operator++() -> LinkedList<T>::iterator&
{
	if (current != nullptr)
		current = current->next;
	return *this;
}

template <typename T>
auto LinkedList<T>::iterator::operator++(int) -> LinkedList<T>::iterator
{
	iterator to_return{*this};
	operator++();

	return to_return;
}

template <typename T>
bool LinkedList<T>::iterator::operator==(const LinkedList<T>::iterator& right) const
{
	return ref == right.ref && (current == nullptr || right.current == nullptr ? current == right.current : current->value == right.current->value);
}

template <typename T>
bool LinkedList<T>::iterator::operator!=(const LinkedList<T>::iterator& right) const
{
	return !operator==(right);
}

template <typename T>
T& LinkedList<T>::iterator::operator*() const
{
	if (current == nullptr)
		throw std::out_of_range{"linked_list::iterator::operator* -- cursor past end"};

	return current->value;
}

template <typename T>
T* LinkedList<T>::iterator::operator->() const
{
	if (current == nullptr)
		throw std::out_of_range{"linked_list::iterator::operator-> -- cursor past end"};

	return &(current->value);
}

// ---
template <typename T>
void LinkedList<T>::deallocate_list(node*& start)
{
	if (start == nullptr)
		return;
	deallocate_list(start->next);
	delete start;
}

template <typename T>
void LinkedList<T>::_ensure_rear()
{
	if (!empty())
	{
		node* current;
		for (current = front; current->next != nullptr; current = current->next)
		{
		}
		rear = current;
	}
}



#endif // LINKED_LIST_HPP
