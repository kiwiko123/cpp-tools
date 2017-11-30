/* Selection algorithms;
 *  - find maximum value of an array
 *  - find second-largest value in an array
 *  - find k-smallest value in an array
 *
 * Author: Geoffrey Ko (2017)
 * Developed using the following configuration:
 *   compiler: clang++
 *             Apple LLVM version 9.0.0 (clang-900.0.38)
 *   version: C++14
 */
#ifndef ALGORITHMS_SELECTION_HPP
#define ALGORITHMS_SELECTION_HPP

#include <array>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <random>
#include <stdexcept>
#include <unordered_map>
#include <vector>


namespace
{
    template <typename InputIterator>
    void bound_check(InputIterator first, InputIterator last)
    {
        if (first == last)
        {
            throw std::out_of_range{"empty range"};
        }
    }

    template <typename InputIterator>
    int brute_force_median(InputIterator first, InputIterator last)
    {
        std::vector<int> temp{first, last};
        std::sort(temp.begin(), temp.end());
        int offset = std::floor(temp.size() / 2);
        int median = temp[offset];
        if (temp.size() % 2 == 0)
        {
            median = (median + temp[offset + 1]) / 2;
        }
        return median;
    }

    template <typename InputIterator>
    int brute_force_select(InputIterator first, InputIterator last, int k)
    {
        std::vector<int> temp{first, last};
        std::sort(temp.begin(), temp.end());
        return temp[k];
    }
}

/* Trivial algorithm to find the maximum value in the sequence [first, last).
 * Θ(n) time.
 * Θ(n - 1) comparisons.
 */
template <typename InputIterator>
int find_maximum(InputIterator first, InputIterator last)
{
    bound_check(first, last);
    int current_max = *(first++);
    while (first != last)
    {
        if (*first > current_max)
        {
            current_max = *first;
        }
        ++first;
    }
    return current_max;
}


/* Tournament selection algorithm to find the second largest element in [first, last).
 * Trivially, the second-largest item can be found by:
 *   1) finding the maximum of [first, last): n - 1 comparisons
 *   2) removing it
 *   3) finding the maximum of the new sequence: n - 2 comparisons
 * This requires Θ(2n - 3) comparisons.
 *
 * Using a tournament tree, the second-largest element is one of the elements that was directly compared with the maximum.
 * By building a bottom-up heap to represent the comparisons, we can:
 *   - find the maximum value using n - 1 comparisons
 *   - find the second-largest value:
 *     - in Θ(log n) time (by definition, heaps are balanced)
 *     - using Θ(⌈log n⌉ - 1) comparisons.
 *
 * Example: [64, 41, 53, 62, 68, 60, 75, 63]
 * build tournament tree:
 * 
 *           75
 *      /          \
 *     64          75
 *   /    \      /    \
 *  64    62    68    75
 *  / \   / \   / \   / \
 * 64 41 53 62 68 60 75 63
 *
 * Build the tree bottom-up - the leaves are each pair of numbers:
 *   (64, 41), (53, 62), (68, 60), (75, 63)
 *
 * Select the "winners" (greater numbers) of each pair:
 *   (64, 62), (68, 75)
 *
 * Continue doing this until the entire tree is built.
 * Descend the tree in Θ(log n) time (considering only nodes of which 75 is a child)
 * to find the maximum value that 75 was compared to.
 * This incurs Θ(⌈log n⌉ - 1) comparisons.
 */
template <typename InputIterator>
int find_second_largest(InputIterator first, InputIterator last)
{
    bound_check(first, last);
    int size = std::distance(first, last);
    if (size == 1)
    {
        return *first;
    }

    std::deque<int> tournament{first, last};    // initially contains just the leaves
    int current_level_size = size;

    // O(n): builds the heap, bottom-up
    while (current_level_size > 1)
    {
        // winners represents the parents of the current level of the tournament-tree
        std::vector<int> winners;
        for (unsigned int i = 0; i < current_level_size; i += 2)
        {
            int winner = (i + 1) < current_level_size ? std::max(tournament[i], tournament[i + 1]) : tournament[i];
            winners.push_back(winner);
        }
        for (auto i = winners.rbegin(); i != winners.rend(); ++i)
        {
            tournament.push_front(*i);
        }
        current_level_size = winners.size();
    }

    const int largest = tournament.front();
    int second_largest = std::numeric_limits<int>::min();
    unsigned int i = 0;

    // descend the winner's competitors to find the second-largest element
    while (i < tournament.size())
    {
        int left_child = 2 * i + 1;
        int right_child = 2 * i + 2;
        if (left_child < tournament.size() && tournament[left_child] == largest)
        {
            second_largest = std::max(second_largest, tournament[right_child]);
            i = left_child;
        }
        else if (right_child < tournament.size() && tournament[right_child] == largest)
        {
            second_largest = std::max(second_largest, tournament[left_child]);
            i = right_child;
        }
        else
        {
            break;
        }
    }

    return second_largest;
}


template <typename InputIterator, typename Selector>
int select_k(InputIterator first, InputIterator last, int k, int median, 
             std::vector<int>& less, std::vector<int>& equal, std::vector<int>& greater,
             const Selector& selector)
{
    while (first != last)
    {
        if (*first < median)
        {
            less.push_back(*first);
        }
        else if (*first > median)
        {
            greater.push_back(*first);
        }
        else
        {
            equal.push_back(*first);
        }
        ++first;
    }

    if (k <= less.size())
    {
        return selector(less.begin(), less.end(), k);
    }
    else if (k <= (less.size() + equal.size()))
    {
        return median;
    }
    else
    {
        return selector(greater.begin(), greater.end(), k - less.size() - equal.size());
    }
}


/* Quick select.
 * Prune-and-search algorithm for finding the k-th smallest value in the iterator range [first, last).
 * Choose an approximate median "m*" randomly from [first, last).
 * Let S be the sequence of items from [first, last).
 * Let L be the sequence of items in S that are less than m*.
 * Let E be the sequence of items in S that are equal to m*.
 * Let G be the sequence of items in S that are greater than m*.
 *
 * Recursively select L, E, or G as appropriate.
 *
 * Worst case: O(n^2) time
 * Best/Average case: O(n) time.
 */
template <typename InputIterator>
int quick_select(InputIterator first, InputIterator last, int k)
{
    bound_check(first, last);
    int size = std::distance(first, last);
    if (size == 1)
    {
        return *first;
    }

    std::random_device rd;
    std::mt19937 engine{rd()};
    std::uniform_int_distribution<int> uid{0, size - 1};
    int random_median_index = uid(engine);
    int random_median = *(first + random_median_index);

    std::vector<int> less;
    std::vector<int> equal;
    std::vector<int> greater;

    while (first != last)
    {
        if (*first < random_median)
        {
            less.push_back(*first);
        }
        else if (*first > random_median)
        {
            greater.push_back(*first);
        }
        else
        {
            equal.push_back(*first);
        }
        ++first;
    }

    if (k <= less.size())
    {
        return quick_select(less.begin(), less.end(), k);
    }
    else if (k <= (less.size() + equal.size()))
    {
        return random_median;
    }
    else
    {
        return quick_select(greater.begin(), greater.end(), k - less.size() - equal.size());
    }
}


template <typename InputIterator>
int deterministic_select(InputIterator first, InputIterator last, int k)
{
    int size = std::distance(first, last);
    if (size <= 5)
    {
        return brute_force_select(first, last, k);
    }

    const double group_factor = 5.0;
    const int n_groups = std::ceil(size / group_factor);
    const double median_of_n_groups = n_groups / static_cast<double>(2);
    std::vector<int> medians(n_groups);
    std::vector<std::vector<int>> groups(n_groups);
    std::for_each(groups.begin(), groups.end(), [&group_factor](std::vector<int>& group){ group = std::vector<int>(group_factor); });

    // 1. divide into ⌈n/5⌉ groups
    auto current = first;
    for (unsigned int i = 0; i < size; ++i)
    {
        int group_factor_i = group_factor;
        groups[std::floor(i / group_factor)][i % group_factor_i] = *(current++);
    }

    // 2. find the medians of each group, again using brute force
    for (unsigned int i = 0; i < n_groups; ++i)
    {
        const std::vector<int>& target = groups[i];
        medians[i] = brute_force_select(target.begin(), target.end(), std::floor(median_of_n_groups));
    }

    // 3. compute m* (median-of-medians)
    int median_of_medians = deterministic_select(medians.begin(), medians.end(), std::floor(median_of_n_groups));

    // 4. parition into L, E, G
    std::vector<int> less;
    std::vector<int> equal;
    std::vector<int> greater;

    while (first != last)
    {
        if (*first < median_of_medians)
        {
            less.push_back(*first);
        }
        else if (*first > median_of_medians)
        {
            greater.push_back(*first);
        }
        else
        {
            equal.push_back(*first);
        }
        ++first;
    }

    if (k <= less.size())
    {
        return deterministic_select(less.begin(), less.end(), k);
    }
    else if (k <= (less.size() + equal.size()))
    {
        return median_of_medians;
    }
    else
    {
        return deterministic_select(greater.begin(), greater.end(), k - less.size() - equal.size());
    }
}

#endif // ALGORITHMS_SELECTION_HPP
