#ifndef PROJECT2_MLF_QUEUE_HPP
#define PROJECT2_MLF_QUEUE_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <vector>
#include "algorithm_exception.hpp"



template <typename T>
class MLFQueue
{
private:
    typedef std::function<bool(const T&, const T&)> MLFComparator;

public:
    explicit MLFQueue(const MLFComparator& comp);

    operator bool() const;

    template <typename Tother>
    friend std::ostream& operator<<(std::ostream& os, const MLFQueue<Tother>& queue);

    int size() const;
    bool empty() const;
    bool contains(const T& item) const;

    T& top();
    const T& top() const;
    T& get(const T& item);

    void push(const T& item);
    void pop();

private:
    std::vector<T> heap;
    MLFComparator comparator;
};


template <typename T>
MLFQueue<T>::MLFQueue(const MLFComparator& comp)
    : comparator{comp}
{
}

template <typename T>
MLFQueue<T>::operator bool() const
{
    return !empty();
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const MLFQueue<T>& queue)
{
    os << "MLFQueue(";
    if (queue)
    {
        MLFQueue<T> copy{queue};
        os << copy.top();
        copy.pop();
        while (copy)
        {
            os << ", " << copy.top();
            copy.pop();
        }
    }
    os << ")";
    return os;
}

template <typename T>
int MLFQueue<T>::size() const
{
    return heap.size();
}

template <typename T>
bool MLFQueue<T>::empty() const
{
    return heap.empty();
}

template <typename T>
bool MLFQueue<T>::contains(const T& item) const
{
    auto position = std::find(heap.begin(), heap.end(), item);
    return position != heap.end();
}

template <typename T>
T& MLFQueue<T>::top()
{
    return heap.front();
}

template <typename T>
const T& MLFQueue<T>::top() const
{
    return heap.front();
}

template <typename T>
T& MLFQueue<T>::get(const T& item)
{
    if (!contains(item))
    {
        throw AlgorithmException{"MLFQueue<T>::get - item not found in heap"};
    }
    auto position = std::find(heap.begin(), heap.end(), item);
    return *position;
}

template <typename T>
void MLFQueue<T>::push(const T& item)
{
    heap.push_back(item);
    std::push_heap(heap.begin(), heap.end(), comparator);
}

template <typename T>
void MLFQueue<T>::pop()
{
    std::pop_heap(heap.begin(), heap.end(), comparator);
    heap.pop_back();
}


#endif //PROJECT2_MLF_QUEUE_HPP
