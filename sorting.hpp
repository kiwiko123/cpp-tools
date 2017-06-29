#ifndef _SORTING_HPP
#define _SORTING_HPP

#include <functional>
#include <memory>
#include <utility>
#include <math.h>


namespace
{
	unsigned int ndigits(int i)
	{
		return i == 0 ? 1 : floor(log10(abs(i))) + 1;
	}

	int radix_sort_on_pass(int key, int pass, unsigned int* iterations, int radix)
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


/* O(N^2) sorting algorithm.
 * Insertion sort maintains 2 portions of the target array: [sorted | unsorted].
 * At each iteration, the cursor '|' moves up one position, 
 * and moves 1 value from the unsorted portion to its correct position in the sorted portion,
 * by starting from the end of the sorted portion and stably swapping it towards the front of the array until it is appropriately positioned.
 * As such, insertion sort is a stable, in-place algorithm.
 * For already sorted or nearly-sorted input containers, insertion sort approaches linear time.
 */
template <typename BidirectionalIterator, typename T>
void insertion_sort(BidirectionalIterator first, BidirectionalIterator last, T* target, const std::function<bool(const T&, const T&)>& less_than = std::less<T>())
{
	unsigned int i = 0;
	for (BidirectionalIterator current = first; current != last; ++current)
	{
		target[i] = *current;
		for (int back = i; back > 0 && less_than(target[back], target[back - 1]); --back)
		{
			std::swap(target[back], target[back - 1]);
		}
		++i;
	}
}


/* O(N) sorting algorithm for integer based keys.
 * first, last are iterators to the container needed to be sorted.
 * key is a unary function which operates on a SINGLE ITEM within said container;
 *   -- e.g., *first dereferences to a value of type T --
 *   it returns an orderable integer identifier based on the element,
 * target_array is an array, assumed to be of the same size as the container, 
 *   in which the sorted elements will be placed.
 * range is the maximum value of the integers to be sorted.
 */
template <typename InputIterator, typename UnaryKey, typename T>
void counting_sort(InputIterator first, InputIterator last, const UnaryKey& key, T* target_array, unsigned int range)
{
	std::unique_ptr<int[]> occurrences{new int[range]};
	std::unique_ptr<int[]> offsets{new int[range]};
	for (unsigned int i = 0; i < range; ++i)
	{
		occurrences[i] = 0;
		offsets[i] = 0;
	}

	InputIterator restore = first;
	while (first != last)
	{
		++(occurrences[key(*first)]);
		++first;
	}

	for (unsigned int i = 1; i < range; ++i)
	{
		offsets[i] = offsets[i - 1] + occurrences[i - 1];
	}

	while (restore != last)
	{
		int k = key(*restore);
		target_array[offsets[k]] = *restore;
		++(offsets[k]);
		++restore;
	}
}


template <typename InputIterator, typename UnaryKey, typename T>
void radix_sort(InputIterator first, InputIterator last, const UnaryKey& key, T* target_array, unsigned int radix)
{
	unsigned int pass = 0;
	unsigned int max_iterations = 1;

	while (pass++ < max_iterations)
	{
		unsigned int* addr = &max_iterations;
		counting_sort(first, last, [pass, &key, addr, radix](const T& item) -> int { return radix_sort_on_pass(key(item), pass, addr, radix); }, target_array, radix);
	}
}


#endif // SORTING_HPP
