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
 *   - find the second-largest value in O(log n) time
 *   - find the second-largest value by searching the tournament-tree using ⌈log n⌉ - 1 comparisons.
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
 * (64, 41), (53, 62), (68, 60), (75, 63)
 *
 * Select the "winners" (greater numbers) of each pair.
 * Continue doing this until the entire tree is built.
 * Descend the tree in O(log n) time to find the maximum value that 75 was compared to.
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


/* Quick select.
 * Prune-and-search algorithm for finding the k-th smallest value in the iterator range [first, last).
 * Choose and approximate median "m*" randomly from [first, last).
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

#endif // ALGORITHMS_SELECTION_HPP
