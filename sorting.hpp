// Templated header file defining function implementations of various sorting algorithms.
// Supporting the C++11 (and up) standard, all of these implementations take in iterators to a container as arguments.
// Currently, many of the functions place the sorted values into a new array (typically of type T*).
// In the future, I aim to streamline all of them by directly sorting the values of the container referred to by the input iterators.
//
// Author: Geoffrey Ko (2017)
#ifndef _SORTING_HPP
#define _SORTING_HPP

#include <algorithm>
#include <functional>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <utility>
#include <math.h>


// Anonymous namespace defining helper functions.
namespace
{
    /* Returns the number of digits in i.
     */
    unsigned int ndigits(int i)
    {
        return i == 0 ? 1 : floor(log10(abs(i))) + 1;
    }

    /* Helper function for radix_sort.
     * Updates iterations based on the number of digits in key.
     * Returns the significant digit in key of the current pass.
     */
    template <typename T>
    int radix_sort_on_pass(const T& key, int pass, unsigned int* iterations, int radix)
    {
        unsigned int d = ndigits(key);
        if (d > *iterations)
        {
            *iterations = d;
        }
        unsigned int least_sig_digit = key % (int)pow(radix, pass);
        return least_sig_digit / pow(radix, pass - 1);
    }
}


// ================================================
// HELPER FUNCTIONS
// ================================================
template <typename InputIterator>
InputIterator iterator_max(InputIterator a, InputIterator b)
{
    return *a >= *b ? a : b;
}

template <typename InputIterator>
InputIterator iterator_min(InputIterator a, InputIterator b)
{
    return *a <= *b ? a : b;
}

template <typename RandomAccessIterator>
RandomAccessIterator median_of_three(RandomAccessIterator first, RandomAccessIterator last)
{
    int size = std::distance(first, last);
    if (size < 3)
    {
        return first;
    }
    auto mid = first + std::floor(size / 2);
    return iterator_max(iterator_min(first, mid), iterator_min(iterator_max(first, mid), last - 1));
}

template <typename RandomAccessIterator>
RandomAccessIterator partition(RandomAccessIterator first, RandomAccessIterator last)
{
    // TODO: intelligently choose pivot
    auto pivot = first;
    for (auto k = first + 1; k < last; ++k)
    {
        if (*k < *first)
        {
            std::swap(*(++pivot), *k);
        }
    }
    std::swap(*first, *pivot);
    return pivot;
}



// Comparison-based Sorting Algorithms
// -----------------------------------

/* Selection sort.
 * Starts from index i = (n - 1) and finds the maximum value from [0, i], swapping that with the value at position i.
 * Finding the maximum at each iteration incurs O(n) time;
 * Performing this operation n times results in Θ(n^2) comparisons.
 *
 * Θ(n^2) time.
 * O(1) extra space.
 * Unstable.
 */
template <typename BidirectionalIterator>
void selection_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    for (auto i = last - 1; i != first; --i)
    {
        auto temp = i;
        auto position_of_max = std::max_element(first, ++temp);
        std::swap(*i, *position_of_max);
    }
}

/* Insertion sort.
 * Maintains 2 portions of the target array: [sorted | unsorted].
 * At each iteration, the cursor '|' moves up one position, 
 * and moves 1 value from the unsorted portion to its correct position in the sorted portion,
 * by starting from the end of the sorted portion and swapping it with its previous neighbor 
 * towards the front of the array until it is appropriately positioned.
 * As such, insertion sort maintains the original ordering of equivalent keys.
 * For already sorted or nearly-sorted input containers, insertion sort approaches linear time,
 * as the inner loop will immediately break when the value is already greater than or equal to its left adjacent neighbor.
 *
 * Worst/Average case: O(n^2) time.
 * Best case: Ω(n) time.
 * O(1) extra space.
 * Stable.
 */
template <typename BidirectionalIterator>
void insertion_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    for (BidirectionalIterator current = first; current != last; ++current)
    {
        auto left_neighbor = current;
        auto back = left_neighbor--;
        while (back != first && *back < *left_neighbor)
        {
            std::swap(*(back--), *(left_neighbor--));
        }
    }
}

/* Quick sort.
 * Divide-and-conquer algorithm that splits the input sequence into 2 halves,
 * choosing a "pivot" element and moving all elements less than the pivot to its left,
 * and all elements greater than the pivot to its right,
 * then recursively sorts both halves of the sequence.
 *
 * If a good pivot is chosen (i.e., a value close to the middle of the range),
 * quicksort empirically performs well.
 * If a bad pivot is chosen (close to the max/min of the range), then it will approach quadratic time -
 * this is because it will perform n comparisons in attempting to move elements to the appropriate side of the pivot,
 * but will iterate through without having accomplished any re-ordering, thus effectively invoking a recursive call
 * on one subsequence of size 1, and another on one of size (n - 1).
 *
 * Worst case: O(n^2) time.
 * Best/Average case: O(nlog n) time.
 * Ω(log n) extra space (recursive stack space).
 * Unstable.
 */
template <typename RandomAccessIterator>
void quick_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    if (first < last)
    {
        auto pivot = partition(first, last);
        quick_sort(first, pivot);
        quick_sort(pivot + 1, last);
    }
}


// Linear-time Sorting Algorithms
// ------------------------------

/* O(N) sorting algorithm for integer based keys.
 * first, last are iterators to the container needed to be sorted.
 * key is a unary function which operates on a SINGLE ITEM within said container;
 *   -- e.g., *first dereferences to a value of type T --
 *   it returns an orderable integer identifier based on the element,
 * target_array is an array, assumed to be of the same size as the container, 
 *   in which the sorted elements will be placed.
 * range is the maximum value of the integers to be sorted.
 *
 * Counting sort is NOT an in-place algorithm, as it utilizes Θ(2R) + O(N) space 
 *   (for temporary arrays to count occurrences and offsets, and map for tracking overwritten values),
 * where R is the range.
 * Runs in Θ(2N + 2R) time, where N is the number of items in the input container, and R is the range;
 * generalizing to O(N) when n is significantly greater than R.
 */
template <typename RandomAccessIterator, typename UnaryKey>
void counting_sort(RandomAccessIterator first, RandomAccessIterator last, const UnaryKey& key, unsigned int range)
{
    std::unique_ptr<int[]> occurrences{new int[range]};
    std::unique_ptr<int[]> offsets{new int[range]};
    for (unsigned int i = 0; i < range; ++i)
    {
        occurrences[i] = 0;
        offsets[i] = 0;
    }

    // count how many times each integer key in the container appears
    for (RandomAccessIterator current = first; current != last; ++current)
    {
        ++occurrences[key(*current)];
    }

    // calculate offsets for each key;
    // determine where each key belongs in the sorted order
    for (unsigned int i = 1; i < range; ++i)
    {
        offsets[i] = offsets[i - 1] + occurrences[i - 1];
    }

    occurrences.reset();                    // unneeded after populating offsets array
    unsigned int i = 0;                     // current iteration number
    std::unordered_map<int, int> table;     // track values from the container as the original elements are overwritten

    // sort elements in the container
    for (RandomAccessIterator current = first; current != last; ++current)
    {
        int k;
        unsigned int off;
        const auto found = table.find(i);

        if (found == table.end())
        {
            k = key(*current);
            off = offsets[k];
            table[off] = *(first + off);
            *(first + off) = *current;
        }
        else
        {
            int index = found->second;
            table.erase(i);        // remove after lookup, to reduce linear space
            k = key(index);
            off = offsets[k];
            table[off] = *(first + off);
            *(first + off) = index;
        }

        ++offsets[k];
        ++i;
    }
}

/* Overloaded function for performing a counting sort on a container of integers.
 */
template <typename RandomAccessIterator>
void counting_sort(RandomAccessIterator first, RandomAccessIterator second, unsigned int range)
{
	counting_sort(first, second, [](int i){ return i; }, range);
}



/* O(N) sorting algorithm for integer based keys.
 * Sorts integer keys by least-significant digits, which are obtained in Θ(1) time through arithmetic operations.
 * The number of values each digit can have is called the radix - 
 *  i.e., for typical numbers [0-9], the radix would be 10, as there are possible 10 digits between 0 and 9 (inclusive).
 * Performs k counting sorts, where k is the number of digits of each value in the array (e.g., 100 has 3 digits).
 *
 * An example is as follows:
 *   {131, 124, 100} would make 3 passes:
 *      1) {100, 131, 124}
 *            ^    ^    ^
 *      2) {100, 124, 131}
 *           ^    ^    ^
 *      3) {100, 124, 131}
 *          ^    ^    ^
 * 
 * Placing a tight bound on its operations, radix sort runs in Θ(d(n + r)) time, where:
 *    - d = number of digits
 *    - n = number of items in the input container
 *    - r = radix
 * which, for large sets of numbers, approaches linear time.
 */
template <typename RandomAccessIterator, typename UnaryKey, typename T>
void radix_sort(RandomAccessIterator first, RandomAccessIterator last, const UnaryKey& key, unsigned int radix, T* dump)
{
    unsigned int pass = 0;
    unsigned int max_iterations = 1;

    while (pass++ < max_iterations)
    {
        unsigned int* addr = &max_iterations;
        counting_sort(first, last, [pass, &key, addr, radix](const T& item) -> int { return radix_sort_on_pass(key(item), pass, addr, radix); }, radix);
    }
}


#endif // SORTING_HPP
