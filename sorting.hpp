#ifndef _SORTING_HPP
#define _SORTING_HPP

#include <functional>
#include <memory>
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
