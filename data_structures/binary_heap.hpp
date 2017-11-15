/* A binary heap is a special type of binary tree.
 * It follows a strict invariant, as per its comparator, which defaults to std::greater<T>.
 * A binary heap with such a comparator is called a Max Heap, whose invariant is that
 * every node is greater than all of its descendants.
 * Additionally, nodes are inserted into the bottom-most, left-most branch,
 * guaranteeing that heaps will never be degenerate,
 * and thus always retain logarithmic height with respect to the number of elements 'n'.
 *
 * Retrieving the top element incurs Θ(1) time.
 * Insertion and extracting the top element incur O(log n) time,
 *   as modifying the heap requires the invariant to be restored.
 * Checking if an element is contained, or erasing an arbitrary item incur O(n) time.
 *
 * Iterators produce items in no particular order;
 * the only way to produce items in order is to extract (retrieve and erase) every element.
 * Additionally, only const_iterators are supported, as modifying items would require the invariant be restored.
 *
 * Binary heaps make efficient priority queue implementation, as the top element can be retrieved in constant time,
 * and inserting elements incur logarithmic time.
 *
 * Author: Geoffrey Ko (2017)
 * Developed using the following configuration:
 *   compiler: clang++
 *             Apple LLVM version 9.0.0 (clang-900.0.38)
 *   version: C++14
 */
#ifndef DATA_STRUCTURES_BINARY_HEAP_HPP
#define DATA_STRUCTURES_BINARY_HEAP_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#include <cmath>


template <typename T, typename Comparator = std::greater<T>>
class BinaryHeap
{
public:
    BinaryHeap();

    /* Converts elements produced from InputIterator into an ordered heap.
     * O(n) optimized (inserts all items linearly, then heapifies itself).
     */
    template <typename InputIterator>
    BinaryHeap(InputIterator first, InputIterator last);

    template <typename E, typename C>
    friend std::ostream& operator<<(std::ostream& os, const BinaryHeap<E, C>& h);

    /* Returns the number of elements in the heap.
     * Θ(1) time.
     */
    int size() const;

    /* Returns true if the heap is empty.
     * Θ(1) time.
     */
    bool empty() const;

    /* Returns true if element is in the heap, or false if not.
     * Linear search - O(n) time.
     */
    bool contains(const T& element) const;

    /* Returns a const-reference to the value at the root element.
     * Θ(1) time.
     */
    const T& top() const;

    /* Returns and erases the value at the root element.
     * O(log n) time.
     */
    T extract();

    /* Inserts element into the heap.
     * O(log n) time.
     */
    void insert(const T& element);


protected:
    /* Erases the element at index i.
     * O(log n).
     */
    void erase_at(int i);

    /* Returns true if index i is in the range [0, size())
     */
    bool in_heap(int i) const;

    static int left_child(int i);
    static int right_child(int i);
    static int parent_of(int i);

private:
    std::vector<T> heap;
    Comparator comp;

    /* Restores heap invariant to the underlying std::vector.
     * O(n) optimized.
     */
    void heapify();

    /* Recursively swaps the element at index i with its parent until the immediate heap invariant is restored.
     * O(log n) time.
     */
    void sift_up(int i);

    /* Recursively swaps the element at index i with its larger child until the immediate heap invariant is restored.
     * O(log n) time.
     */
    void sift_down(int i);

    /* Throws std::runtime_error if the heap is empty.
     */
    void check_empty(const std::string& function_name) const;


    // Iterator Implementation
    template <typename UnqualifiedT = typename std::remove_const<T>::type>
    class iterator_type : public std::iterator<std::bidirectional_iterator_tag, UnqualifiedT, std::ptrdiff_t, T*, T&>
    {
    public:
        iterator_type(const BinaryHeap<T, Comparator>* over, unsigned int index);

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
        T* operator->();

    private:
        const BinaryHeap<T, Comparator>* ref;	// raw pointer (instead of smart pointer) because no dynamic allocation or modifying operations are used
        unsigned int traversed;

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
    // BinaryHeap does not support non-const iterators, as mutated values would invalidate the heap invariant.
    auto begin() const -> iterator;
    auto end() const -> iterator;
};


template <typename T, typename Comparator>
BinaryHeap<T, Comparator>::BinaryHeap()
{
}

template <typename T, typename Comparator>
template <typename InputIterator>
BinaryHeap<T, Comparator>::BinaryHeap(InputIterator first, InputIterator last)
{
    while (first != last)
    {
        heap.push_back(*(first++));
    }
    heapify();
}

template <typename T, typename Comparator>
std::ostream& operator<<(std::ostream& os, const BinaryHeap<T, Comparator>& bh)
{
    os << "BinaryHeap(";
    auto i = bh.heap.begin();
    os << *(i++);
    while (i != bh.heap.end())
    {
        os << ", " << *(i++);
    }
    os << ")";
    return os;
}

template <typename T, typename Comparator>
int BinaryHeap<T, Comparator>::size() const
{
    return heap.size();
}

template <typename T, typename Comparator>
bool BinaryHeap<T, Comparator>::empty() const
{
    return size() == 0;
}

template <typename T, typename Comparator>
bool BinaryHeap<T, Comparator>::contains(const T& element) const
{
    return std::find(heap.begin(), heap.end(), element) != heap.end();
}

template <typename T, typename Comparator>
const T& BinaryHeap<T, Comparator>::top() const
{
    check_empty("empty");
    return heap.front();
}

template <typename T, typename Comparator>
void BinaryHeap<T, Comparator>::insert(const T& element)
{
    heap.push_back(element);
    sift_up(size() - 1);
}

template <typename T, typename Comparator>
T BinaryHeap<T, Comparator>::extract()
{
    check_empty("extract");
    T result = top();
    erase_at(0);
    return result;
}

template <typename T, typename Comparator>
void BinaryHeap<T, Comparator>::erase_at(int i)
{
    check_empty("erase_at");
    if (!in_heap(i))
    {
        throw std::runtime_error{"BinaryHeap::erase_at - index i out of bounds"};
    }
    heap[i] = heap.back();
    heap.pop_back();
    sift_up(i);
    sift_down(i);
}

template <typename T, typename Comparator>
bool BinaryHeap<T, Comparator>::in_heap(int i) const
{
    return i >= 0 && i < size();
}

template <typename T, typename Comparator>
int BinaryHeap<T, Comparator>::left_child(int i)
{
    return 2 * i + 1;
}

template <typename T, typename Comparator>
int BinaryHeap<T, Comparator>::right_child(int i)
{
    return 2 * i + 2;
}

template <typename T, typename Comparator>
int BinaryHeap<T, Comparator>::parent_of(int i)
{
    return std::floor((i - 1) / 2);
}

template <typename T, typename Comparator>
void BinaryHeap<T, Comparator>::sift_up(int i)
{
    int parent = parent_of(i);
    if (in_heap(parent) && comp(heap[i], heap[parent]))
    {
        std::swap(heap[parent], heap[i]);
        sift_up(parent);
    }
}

template <typename T, typename Comparator>
void BinaryHeap<T, Comparator>::sift_down(int i)
{
    int left = left_child(i);
    int right = right_child(i);
    int larger;

    if (in_heap(left) && comp(heap[left], heap[right]))
    {
        larger = left;
    }
    else if (in_heap(right))
    {
        larger = right;
    }
    else
    {
        return;
    }

    if (comp(heap[larger], heap[i]))
    {
        std::swap(heap[larger], heap[i]);
        sift_down(larger);
    }
}

template <typename T, typename Comparator>
void BinaryHeap<T, Comparator>::heapify()
{
    for (int i = parent_of(size() - 1); i >= 0; --i)
    {
        sift_down(i);
    }
}

template <typename T, typename Comparator>
void BinaryHeap<T, Comparator>::check_empty(const std::string& function_name) const
{
    if (empty())
    {
        std::string message = "BinaryHeap::" + function_name + " - heap is empty";
        throw std::runtime_error{message};
    }
}

template <typename T, typename Comparator>
auto BinaryHeap<T, Comparator>::cbegin() const -> const_iterator
{
    return const_iterator{this, 0};
}

template <typename T, typename Comparator>
auto BinaryHeap<T, Comparator>::cend() const -> const_iterator
{
    unsigned int sz = size();		// silence g++ warning about conflicting types (unsigned int vs. int)
    return const_iterator{this, sz};
}

template <typename T, typename Comparator>
auto BinaryHeap<T, Comparator>::begin() const -> iterator
{
    return cbegin();
}

template <typename T, typename Comparator>
auto BinaryHeap<T, Comparator>::end() const -> iterator
{
    return cend();
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::iterator_type(const BinaryHeap<T, Comparator> *over, unsigned int index)
    : ref{over}, traversed{index}
{
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
void BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::bound_check(const std::string& message) const
{
    if (traversed < 0 || traversed >= ref->size())
    {
        throw std::runtime_error{message};
    }
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
bool BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator==(const iterator_type& other) const
{
    return ref == other.ref && traversed == other.traversed;
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
bool BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator!=(const iterator_type& other) const
{
    return !operator==(other);
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
auto BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator++() -> iterator_type<UnqualifiedT>&
{
    if (traversed < ref->size())
    {
        ++traversed;
    }
    return *this;
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
auto BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator++(int) -> iterator_type<UnqualifiedT>
{
    iterator_type<UnqualifiedT> state{*this};
    operator++();
    return state;
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
auto BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator--() -> iterator_type<UnqualifiedT>&
{
    if (traversed > 0)
    {
        --traversed;
    }
    return *this;
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
auto BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator--(int) -> iterator_type<UnqualifiedT>
{
    iterator_type<UnqualifiedT> state{*this};
    operator--();
    return state;
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
UnqualifiedT& BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator*()
{
    bound_check("BinaryHeap::iterator_type::operator*");
    return ref->heap[traversed];
}

template <typename T, typename Comparator>
template <typename UnqualifiedT>
T* BinaryHeap<T, Comparator>::iterator_type<UnqualifiedT>::operator->()
{
    bound_check("BinaryHeap::iterator_type::operator->");
    return &(ref->heap[traversed]);
}


#endif //DATA_STRUCTURES_BINARY_HEAP_HPP
