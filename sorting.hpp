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

	template <typename RandomAccessIterator>
	void merge(RandomAccessIterator first, RandomAccessIterator left_end, RandomAccessIterator right_start, RandomAccessIterator last)
	{
		unsigned int size = std::distance(first, last);
		std::unique_ptr<RandomAccessIterator[]> temp{new RandomAccessIterator[size + 1]};

		auto i = first;
		auto j = right_start;
		for (unsigned int n = 0; n < size; ++n)
		{
			if (i == left_end)
			{
				temp[n] = j++;
			}
			else if (j == last)
			{
				temp[n] = i++;
			}
			else if (*i <= *j)
			{
				temp[n] = i++;
			}
			else
			{
				temp[n] = j++;
			}
		}

		for (unsigned int n = 0; n < size; ++n)
		{
			*(first++) = *(std::move(temp[n]));
		}
	}
}



// Comparison-based Sorting Algorithms
// -----------------------------------

/* O(N^2) sorting algorithm.
 * Insertion sort maintains 2 portions of the target array: [sorted | unsorted].
 * At each iteration, the cursor '|' moves up one position, 
 * and moves 1 value from the unsorted portion to its correct position in the sorted portion,
 * by starting from the end of the sorted portion and swapping it with its previous neighbor 
 * towards the front of the array until it is appropriately positioned.
 * As such, insertion sort is a stable, in-place algorithm.
 * For already sorted or nearly-sorted input containers, insertion sort approaches linear time.
 */
template <typename BidirectionalIterator>
void insertion_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    for (BidirectionalIterator current = first; current != last; ++current)
    {
        BidirectionalIterator left_neighbor = current;
        BidirectionalIterator back = left_neighbor--;
        while (back != first && *back < *left_neighbor)
        {
            std::swap(*(back--), *(left_neighbor--));
        }
    }
}


template <typename RandomAccessIterator>
void merge_sort(RandomAccessIterator first, RandomAccessIterator last)
{
	if (first < last)
	{
		unsigned int size = std::distance(first, last);		// Θ(1) for random access iterators
		auto mid = first + (size / 2);

		merge_sort(first, mid);
		merge_sort(mid + 1, last);

		merge(first, mid, mid + 1, last);
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
