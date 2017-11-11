#ifndef DATA_STRUCTURES_CACHE_LINKED_LIST_HPP
#define DATA_STRUCTURES_CACHE_LINKED_LIST_HPP

// This header defines a data structure called "CacheLinkedList", which is essentially a doubly linked list that gives access to individual nodes.
// Pushing to the list returns the node that was just added, and erasing takes in a node - allowing O(1) removal (provided that you have the target node).
// I wrote this as a supplemental data structure to an LRU Cache implementation.
// To achieve O(1) insertions and lookups, my LRU Cache maintains a hash table (std::unordered_map) and a linked list (CacheLinkedList).
// Each key in the hash table is associated to a node (pointer) in the linked list - as such, std::list is not a viable option (cannot access individual nodes).

#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <cstddef>

#ifndef DEBUG
#define DEBUG false 
#endif

#define LOGME(m) if (DEBUG) { std::cout << __FILE__ << std::endl << "  " << __func__ << std::endl << "    \"" << (m) << "\"" << std::endl; }


template <typename T>
class CacheLinkedList 
{
private:
    struct Node;
public:
    typedef std::shared_ptr<Node> NodePointer;
private:
    struct Node
    {
        T value;
        NodePointer previous;
        NodePointer next;

        Node(const T& v, NodePointer p, NodePointer n);
        Node(const T& v);
    };


public:
    CacheLinkedList();

	template <typename InputIterator>
	CacheLinkedList(InputIterator first, InputIterator last);	

    template <typename QualifiedT>
    friend std::ostream& operator<<(std::ostream& os, const CacheLinkedList<QualifiedT>& c);

    int size() const;
    bool empty() const;

    NodePointer front() const;
    NodePointer back() const;
	NodePointer push_front(const T& value);
    NodePointer push_back(const T& value);
    void erase_node(NodePointer node);
    void pop_front();
    void pop_back();

private:
    NodePointer head;
    NodePointer tail;
	unsigned int length;

    template <typename UnqualifiedT = typename std::remove_const<T>::type>
    class iterator_type : public std::iterator<std::bidirectional_iterator_tag, UnqualifiedT, std::ptrdiff_t, T*, T&>
    {
    public:
        iterator_type(const CacheLinkedList<T>* linked_list, NodePointer start, unsigned int visited);

        bool operator==(const iterator_type& other) const;
        bool operator!=(const iterator_type& other) const;
        auto operator++() -> iterator_type&;
        auto operator++(int) -> iterator_type;
        auto operator--() -> iterator_type&;
        auto operator--(int) -> iterator_type;
        UnqualifiedT& operator*();
        NodePointer operator->();

    private:
        const CacheLinkedList<T>* ref;
        NodePointer current;
        unsigned int traversed;

        void bound_check(const std::string& message) const;
    };

public:
    typedef iterator_type<T> iterator;
    typedef iterator_type<const T> const_iterator;

    auto begin() const -> iterator;
    auto end() const -> iterator;
    auto cbegin() const -> const_iterator;
    auto cend() const -> const_iterator;
};


template <typename T>
CacheLinkedList<T>::Node::Node(const T& v, NodePointer p, NodePointer n)
    : value{v}, previous{p}, next{n}
{
}

template <typename T>
CacheLinkedList<T>::Node::Node(const T& v) : Node{v, nullptr, nullptr}
{
}

template <typename T>
CacheLinkedList<T>::CacheLinkedList()
    : head{nullptr}, tail{nullptr}, length{0}
{
}

template <typename T>
template <typename InputIterator>
CacheLinkedList<T>::CacheLinkedList(InputIterator first, InputIterator last) : CacheLinkedList<T>{}
{
	while (first != last)
	{
		push_back(*(first++));
	}
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const CacheLinkedList<T>& c)
{
    os << "CacheLinkedList(";
    if (!c.empty())
    {
        typename CacheLinkedList<T>::NodePointer current = c.front();
        os << current->value;
        while ((current = current->next))
        {
            os << ", " << current->value;
        }
    }
    os << ")";
    return os;
}

template <typename T>
int CacheLinkedList<T>::size() const
{
	return length;
}

template <typename T>
bool CacheLinkedList<T>::empty() const
{
	return size() == 0;
}

template <typename T>
typename CacheLinkedList<T>::NodePointer CacheLinkedList<T>::front() const
{
    return head;
}

template <typename T>
typename CacheLinkedList<T>::NodePointer CacheLinkedList<T>::back() const
{
    return tail;
}

template <typename T>
typename CacheLinkedList<T>::NodePointer CacheLinkedList<T>::push_back(const T& value)
{
    if (empty())
    {
        head = tail = NodePointer{new Node{value}};
    }
    else
    {
        tail->next = NodePointer{new Node{value, tail, nullptr}};
        tail = tail->next;
    }
	++length;
    return tail;
}

template <typename T>
typename CacheLinkedList<T>::NodePointer CacheLinkedList<T>::push_front(const T& value)
{
	if (empty())
	{
		return push_back(value);
	}
	else
	{
		auto current = NodePointer{new Node{value, nullptr, head}};
		head = current;
		++length;
	}
	return head;
}

template <typename T>
void CacheLinkedList<T>::erase_node(NodePointer node)
{
    if (!node)
    {
        throw std::runtime_error{"CacheLinkedList<T>::erase_node - cannot erase nullptr"};
    }
    auto previous = node->previous;
    auto next = node->next;
    if (previous)
    {
        previous->next = next;
    }
    else if (next)  // node is the head, and it has a next value
    {
        head = next;
        head->previous.reset();
    }
    else    // node is the head, and ONLY item in the linked list
    {
        head = tail = NodePointer{};
    }
}

template <typename T>
void CacheLinkedList<T>::pop_front()
{
    erase(head);
}

template <typename T>
void CacheLinkedList<T>::pop_back()
{
    erase(tail);
}

template <typename T>
auto CacheLinkedList<T>::begin() const -> iterator
{
    return iterator{this, head, 0};
}

template <typename T>
auto CacheLinkedList<T>::end() const -> iterator
{
	unsigned int sz = size();	// silence g++ warning about unsigned int -> int conversion
    return iterator{this, nullptr, sz};
}

template <typename T>
auto CacheLinkedList<T>::cbegin() const -> const_iterator
{
    return const_iterator{this, head, 0};
}

template <typename T>
auto CacheLinkedList<T>::cend() const -> const_iterator
{
	unsigned int sz = size();
    return const_iterator{this, nullptr, sz};
}

template <typename T>
template <typename UnqualifiedT>
CacheLinkedList<T>::iterator_type<UnqualifiedT>::iterator_type(const CacheLinkedList<T>* linked_list, NodePointer start, unsigned int visited)
    : ref{linked_list}, current{start}, traversed{visited}
{
}

template <typename T>
template <typename UnqualifiedT>
bool CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator==(const iterator_type<UnqualifiedT>& other) const
{
	LOGME(traversed);
    return ref == other.ref && traversed == other.traversed && (current ? other.current.operator bool() : !(current || other.current));
}

template <typename T>
template <typename UnqualifiedT>
bool CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator!=(const iterator_type<UnqualifiedT>& other) const
{
    return !operator==(other);
}

template <typename T>
template <typename UnqualifiedT>
auto CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator++() -> iterator_type<UnqualifiedT>&
{
	LOGME(traversed);
    if (current)
    {
        current = current->next;
        ++traversed;
    }
    return *this;
}

template <typename T>
template <typename UnqualifiedT>
auto CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator++(int) -> iterator_type<UnqualifiedT>
{
    iterator_type state{*this};
    operator++();
    return state;
}

template <typename T>
template <typename UnqualifiedT>
auto CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator--() -> iterator_type<UnqualifiedT>&
{
    if (!current)
    {
        current = ref->tail;
    }
    if (current)
    {
        current = current->previous;
        --traversed;
    }
    return *this;
}

template <typename T>
template <typename UnqualifiedT>
auto CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator--(int) -> iterator_type<UnqualifiedT>
{
    iterator_type state{*this};
    operator--();
    return state;
}

template <typename T>
template <typename UnqualifiedT>
UnqualifiedT& CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator*()
{
    bound_check("CacheLinkedList::iterator_type::operator* - iterator past end");
    return current->value;
}

template <typename T>
template <typename UnqualifiedT>
typename CacheLinkedList<T>::NodePointer CacheLinkedList<T>::iterator_type<UnqualifiedT>::operator->()
{
    bound_check("CacheLinkedList::iterator_type::operator-> iterator past end");
    return current;
}

template <typename T>
template <typename UnqualifiedT>
void CacheLinkedList<T>::iterator_type<UnqualifiedT>::bound_check(const std::string& message) const
{
    if (!current)
    {
        throw std::runtime_error{message};
    }
}


#undef DEBUG
#endif // DATA_STRUCTURES_CACHE_LINKED_LIST_HPP
