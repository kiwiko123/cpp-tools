/* Templated header file defining function implementations of various sorting algorithms.
 * Supporting the C++11 (and up) standard, all of these implementations take in iterators of a container as arguments.
 *
 * Author: Geoffrey Ko (2017)
 * Developed using the following configuration:
 *   compiler: clang++
 *             Apple LLVM version 9.0.0 (clang-900.0.38)
 *   version: C++14
 */
#ifndef _ALGORITHMS_SORTING_HPP
#define _ALGORITHMS_SORTING_HPP

#include <algorithm>
#include <iterator>
#include <memory>
#include <utility>
#include <vector>
#include <cmath>


// ================================================
// HELPER FUNCTIONS
// ================================================
/* Returns the iterator that points to the maximum value between *a and *b.
 */
template <typename InputIterator>
InputIterator iterator_max(InputIterator a, InputIterator b)
{
    return *a >= *b ? a : b;
}

/* Returns the iterator that points to the minimum value between *a and *b.
 */
template <typename InputIterator>
InputIterator iterator_min(InputIterator a, InputIterator b)
{
    return *a <= *b ? a : b;
}

/* Helper function for partition;
 * Returns the median value between the first, middle, and last items in [first, last).
 */
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

/* Helper function for quick_sort;
 * Reorders keys so that all items less than the pivot appear before it,
 * and all items greater than or equal to it appear after.
 * Returns the iterator to the selected pivot value.
 */
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

// ================================================
// END HELPER FUNCTIONS
// ================================================



// ================================================
// Comparison-based Sorting Algorithms
// ================================================

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


// ===================================================
// Linear-time, Address-Calculation Sorting Algorithms
// ===================================================

/* Counting sort, for integer-based keys.
 * Let n be the number of items in the range [first, last), and k be the range.
 * Range refers to the upper bound of integers in the inclusive bounds of [0, k] -
 * i.e., all values in [first, last) are integers between [0, k].
 *
 * Constructs an auxiliary "locator" array to determine the indices that each value should lie in,
 * based on the determined indices of each preceding value.
 * A temporary std::vector is also created that stores a copy of the range [first, last),
 * so that the input iterators can be directly modified.
 *
 * Θ(2n + 2k) = O(n) time, when k <= n.
 * Θ(n + k) = O(n) extra space.
 * Stable.
 */
template <typename RandomAccessIterator>
void counting_sort(RandomAccessIterator first, RandomAccessIterator last, unsigned int range)
{
    std::unique_ptr<int[]> locator{new int[range + 1]};
    for (unsigned int i = 0; i <= range; ++i)
    {
        locator[i] = 0;
    }
    for (auto current = first; current != last; ++current)
    {
        locator[*current] += 1;
    }
    for (unsigned int i = 1; i <= range; ++i)
    {
        locator[i] += locator[i - 1];
    }

    std::vector<int> temp{first, last};
    for (auto current = temp.crbegin(); current != temp.crend(); ++current)
    {
        // decrement first because locator values are built 1-based, but output range is 0-based
        locator[*current] -= 1;
        *(first + locator[*current]) = *current;
    }
}

/* Counting sort, but automatically determines the maximum value in [first, last).
 */
template <typename RandomAccessIterator>
void counting_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    auto position_of_max = std::max_element(first, last);
    if (position_of_max != last)
    {
        counting_sort(first, last, *position_of_max);
    }
}

/* Bucket sort.
 * Divides the integer-based keys in [first, last) into contiguous std::vector subarrays (buckets).
 * Let n be the number of items in [first, last), k be the range, and b be the number of buckets.
 * Distributes the items into equal-sized buckets, based on specific cutoff points (k / b).
 * Sorts each bucket using insertion sort.
 * Merges all the sorted buckets back into the original iterators.
 *
 * Let s(i) be the number of items in bucket i.
 * Θ(n + s(0)^2 + s(1)^2 + ... + s(b)^2 + b) = O(n) time, when b <= n.
 * O(n + b) extra space.
 * Stable - keys are added to each bucket in the same order they appear.
 */
template <typename BidirectionalIterator, typename BucketContainer = std::vector<int>>
void bucket_sort(BidirectionalIterator first, BidirectionalIterator last, int range, int number_of_buckets)
{
    std::unique_ptr<BucketContainer[]> buckets{new std::vector<int>[number_of_buckets]};
    int cutoff = range / number_of_buckets;
    for (auto current = first; current != last; ++current)
    {
        int bucket_index = std::floor(*current / cutoff);
        buckets[bucket_index].push_back(*current);
    }

    for (int i = 0; i < number_of_buckets; ++i)
    {
        BucketContainer& bucket = buckets[i];
        insertion_sort(bucket.begin(), bucket.end());

        for (const auto& element: bucket)
        {
            *(first++) = element;
        }
    }
}

#endif // _ALGORITHMS_SORTING_HPP
