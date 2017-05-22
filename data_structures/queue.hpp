#ifndef DATA_STRUCTURES_QUEUE_HPP
#define DATA_STRUCTURES_QUEUE_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <string>


template <typename T>
class Queue
{
public:
    Queue();

    template <typename InputIterator>
    Queue(InputIterator first, InputIterator last);

    template <typename Tother>
    friend std::ostream& operator<<(std::ostream& os, const Queue<Tother>& queue);

    bool operator==(const Queue<T>& other) const;
    bool operator!=(const Queue<T>& other) const;
    operator bool() const;

    int size() const;
    bool empty() const;
    const T& front() const;
    bool contains(const T& item) const;

    T& front();
    void push(const T& item);
    void pop();


    class iterator;
    auto begin() const -> iterator;
    auto end() const -> iterator;

    class iterator : public std::iterator<std::bidirectional_iterator_tag, T>
    {
    public:
        iterator(Queue<T>* queue_ptr, typename std::list<T>::const_iterator start);

        auto operator++() -> iterator&;
        auto operator++(int) -> iterator;
        auto operator--() -> iterator&;
        auto operator--(int) -> iterator;
        bool operator==(const iterator& other) const;
        bool operator!=(const iterator& other) const;
        const T& operator*() const;
        T* operator->() const;

        friend iterator Queue<T>::begin() const;
        friend iterator Queue<T>::end() const;

    private:
        Queue<T>* ref;
        typename std::list<T>::const_iterator it;
    };

protected:
    std::list<T> array;
};



template <typename T>
Queue<T>::Queue()
{
}

template <typename T>
template <typename InputIterator>
Queue<T>::Queue(InputIterator first, InputIterator last)
{
    for (; first != last; ++first)
    {
        push(*first);
    }
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Queue<T>& queue)
{
    os << "Queue(";
    if (queue)
    {
        auto i = queue.begin();
        os << *(i++);
        for (; i != queue.end(); ++i)
        {
            os << ", " << *i;
        }
    }
    os << ")";
    return os;
}

template <typename T>
bool Queue<T>::operator==(const Queue<T>& other) const
{
    return this == &other || std::equal(begin(), end(), other.begin(), other.end());
}

template <typename T>
bool Queue<T>::operator!=(const Queue<T>& other) const
{
    return !operator==(other);
}

template <typename T>
Queue<T>::operator bool() const
{
    return !empty();
}

template <typename T>
int Queue<T>::size() const
{
    return array.size();
}

template <typename T>
bool Queue<T>::empty() const
{
    return array.empty();
}

template <typename T>
bool Queue<T>::contains(const T& item) const
{
    return std::find(begin(), end(), item) != end();
}

template <typename T>
const T& Queue<T>::front() const
{
    return array.front();
}

template <typename T>
T& Queue<T>::front()
{
    return array.front();
}

template <typename T>
void Queue<T>::push(const T& item)
{
    array.push_back(item);
}

template <typename T>
void Queue<T>::pop()
{
    array.pop_front();
}

template <typename T>
auto Queue<T>::begin() const -> Queue<T>::iterator
{
    return iterator{const_cast<Queue<T>*>(this), array.begin()};
}

template <typename T>
auto Queue<T>::end() const -> Queue<T>::iterator
{
    return iterator{const_cast<Queue<T>*>(this), array.end()};
}

template <typename T>
Queue<T>::iterator::iterator(Queue<T>* queue_ptr, typename std::list<T>::const_iterator start)
    : ref{queue_ptr}, it{start}
{
}

template <typename T>
auto Queue<T>::iterator::operator++() -> Queue<T>::iterator&
{
    ++it;
    return *this;
}

template <typename T>
auto Queue<T>::iterator::operator++(int) -> Queue<T>::iterator
{
    iterator state{*this};
    operator++();
    return state;
}

template <typename T>
auto Queue<T>::iterator::operator--() -> Queue<T>::iterator&
{
    --it;
    return *this;
}

template <typename T>
auto Queue<T>::iterator::operator--(int) -> Queue<T>::iterator
{
    iterator state{*this};
    operator--();
    return state;
}

template <typename T>
bool Queue<T>::iterator::operator==(const iterator& other) const
{
    return ref == other.ref && it == other.it;
}

template <typename T>
bool Queue<T>::iterator::operator!=(const iterator& other) const
{
    return !operator==(other);
}

template <typename T>
const T& Queue<T>::iterator::operator*() const
{
    return *it;
}

template <typename T>
T* Queue<T>::iterator::operator->() const
{
    return &it;
}



#endif //DATA_STRUCTURES_QUEUE_HPP
