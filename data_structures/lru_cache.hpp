#ifndef DATA_STRUCTURES_LRU_CACHE_HPP
#define DATA_STRUCTURES_LRU_CACHE_HPP

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

#define DEBUG true

template <typename T>
class LRUCache
{
private:
    struct Node;
protected:
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
    LRUCache();

    template <typename QualifiedT>
    friend std::ostream& operator<<(std::ostream& os, const LRUCache<QualifiedT>& c);

    virtual bool empty() const = 0;
    virtual int size() const = 0;

protected:
    virtual std::string name() const;

    NodePointer front() const;
    NodePointer back() const;
    NodePointer push_back_node(const T& value);
    void erase_node(NodePointer node);
    void pop_first_node();
    void pop_last_node();

private:
    NodePointer head;
    NodePointer tail;
};


template <typename T>
LRUCache<T>::Node::Node(const T& v, NodePointer p, NodePointer n)
    : value{v}, previous{p}, next{n}
{
}

template <typename T>
LRUCache<T>::Node::Node(const T& v) : Node{v, nullptr, nullptr}
{
}

template <typename T>
LRUCache<T>::LRUCache()
    : head{nullptr}, tail{nullptr}
{
}

template <typename T>
std::string LRUCache<T>::name() const
{
    return "LRUCache";
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const LRUCache<T>& c)
{
    os << c.name() << "(";
    if (!c.empty())
    {
        typename LRUCache<T>::NodePointer current = c.front();
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
typename LRUCache<T>::NodePointer LRUCache<T>::front() const
{
    return head;
}

template <typename T>
typename LRUCache<T>::NodePointer LRUCache<T>::back() const
{
    return tail;
}

template <typename T>
typename LRUCache<T>::NodePointer LRUCache<T>::push_back_node(const T& value)
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
    return tail;
}

template <typename T>
void LRUCache<T>::erase_node(NodePointer node)
{
    if (!node)
    {
        throw std::runtime_error{"LRUCache<T>::erase_node - cannot erase nullptr"};
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
void LRUCache<T>::pop_first_node()
{
    erase_node(head);
}

template <typename T>
void LRUCache<T>::pop_last_node()
{
    erase_node(tail);
}

#undef DEBUG
#endif // DATA_STRUCTURES_LRU_CACHE_HPP
