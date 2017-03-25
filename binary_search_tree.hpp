// This header file defines a Binary Search Tree (BST) class.
// Lookup, insertion, and removal operations are done in O(logN) time
// (or, more accurately, O(height) time).
//
// Public member functions provide a high-level interface for using this class
// as the underlying data structure implementation for a data type such as an ordered map or set.
//
// A BST holds objects of the templated type 'T';
// if no less-than comparator function is supplied,
// it defaults to T::operator< for internal ordering and comparisons.
// Regardless, T::operator== must be implemented for internal 'contains' functionality.
//
// Developed and tested on Apple LLVM version 7.3.0 (clang-703.0.31), C++14
//
// Author: Geoffrey Ko (2017)
#ifndef DATA_STRUCTURES_BINARY_SEARCH_TREE_HPP
#define DATA_STRUCTURES_BINARY_SEARCH_TREE_HPP

#include <algorithm>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>


template <typename T>
class BinarySearchTree
{
private:
    typedef std::function<bool(const T&, const T&)> LessThanComparator;

public:
    // (constructor)
    //   Supply a comparator, or use (default) operator<.
    //   Comparator is a binary function with parameters a, b
    //   which returns true if a is equivocally less than b in the tree's ordering.
    explicit BinarySearchTree(const LessThanComparator& less_than_comparator);
    BinarySearchTree();

    template <typename InputIterator>
    BinarySearchTree(InputIterator first, InputIterator last, const LessThanComparator& less_than_comparator);
    template <typename InputIterator>
    BinarySearchTree(InputIterator first, InputIterator last);
    // -------------

    // Operators
    // /* Returns true if this is not empty, or false if empty */
    operator bool() const;
    // ---------

    // Non-Modifying Member Functions
    /* Returns the number of items in this */
    int size() const;

    /* Calculates and returns the height of the tree */
    int height() const;

    /* Returns true if there are no elements in this */
    bool empty() const;

    /* Returns true if 'item' is in this */
    bool contains(const T& item) const;

    /* Returns the root value of the tree */
    const T& top() const;

    /* Returns a side-rotated representation string of this */
    std::string to_string() const;
    // ------------------------------

    // Modifying Member Functions
    /* Add 'item' to this.
     * Does nothing if item is already contained.
     */
    void push(const T& item);

    /* Remove 'item' from this.
     * Throws std::runtime_error if 'item' is not in this.
     */
    void erase(const T& item);

    /* Removes all values from this */
    void clear();
    // --------------------------



protected:
    struct Node;
    typedef std::shared_ptr<Node> NodePointer;

    struct Node
    {
        T value;
        NodePointer left = nullptr;
        NodePointer right = nullptr;
        NodePointer parent = nullptr;

        /* Returns the number of children this node has. e.g., [0, 2] */
        int children() const;
    };

    int calculate_size(NodePointer start) const;


public:
    // Iterator Accessors
    class const_iterator;
    auto begin() const -> const_iterator;
    auto end() const -> const_iterator;

    class const_iterator : public std::iterator<std::forward_iterator_tag, T>
    {
    public:
        const_iterator(NodePointer start, NodePointer now, int so_far, int length_of);
        const_iterator(NodePointer start, NodePointer now, int length_of);

        auto operator++() -> const_iterator&;
        auto operator++(int) -> const_iterator;
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
        const T& operator*() const;
        const NodePointer operator->() const;

    private:
        bool done() const;
        NodePointer seek_min(NodePointer start);
        void advance();

        NodePointer ref;
        NodePointer current;
        NodePointer last_seen;
        int traversed;
        int cap;
    };



private:
    LessThanComparator comparator;
    NodePointer root;
    int length;

    void print_rotated(std::ostringstream& buf, NodePointer current, const std::string& indent) const;
    int height_of(NodePointer current) const;

    NodePointer locate_node(NodePointer current, const T& item) const;
    NodePointer insert_node(NodePointer current, NodePointer current_parent, const T& item);

    void remove_internal_node_one_child(NodePointer node);
    void remove_internal_node_two_children(NodePointer node);
    void remove_internal_node_of_two_children(NodePointer node, NodePointer which_child);
    void remove_root();
    void remove_this_child(NodePointer parent, NodePointer child);
    NodePointer find_minimum_of(NodePointer node);
    NodePointer find_maximum_of(NodePointer node);
};


template <typename T>
BinarySearchTree<T>::BinarySearchTree(const std::function<bool(const T&, const T&)>& less_than_comparator)
    : comparator{less_than_comparator}, root{nullptr}, length{0}
{
}

template <typename T>
BinarySearchTree<T>::BinarySearchTree()
    : BinarySearchTree<T>{std::less<T>()}
{
}

template <typename T>
template <typename InputIterator>
BinarySearchTree<T>::BinarySearchTree(InputIterator first, InputIterator last,
                                      const LessThanComparator& less_than_comparator)
    : BinarySearchTree<T>{less_than_comparator}
{
    for (; first != last; ++first)
    {
        push(*first);
    }
}

template <typename T>
template <typename InputIterator>
BinarySearchTree<T>::BinarySearchTree(InputIterator first, InputIterator last)
    : BinarySearchTree<T>{}
{
    for (; first != last; ++first)
    {
        push(*first);
    }
}

template <typename T>
BinarySearchTree<T>::operator bool() const
{
    return !empty();
}

template <typename T>
int BinarySearchTree<T>::size() const
{
    return length;
}

template <typename T>
int BinarySearchTree<T>::height() const
{
    return height_of(root);
}

template <typename T>
bool BinarySearchTree<T>::empty() const
{
    return size() == 0;
}

template <typename T>
bool BinarySearchTree<T>::contains(const T& item) const
{
    return static_cast<bool>(locate_node(root, item));
}

template <typename T>
const T& BinarySearchTree<T>::top() const
{
    if (*this)
    {
        return root->value;
    }
    throw std::runtime_error{"BinarySearchTree::top - tree is empty"};
}

template <typename T>
std::string BinarySearchTree<T>::to_string() const
{
    std::ostringstream buf;
    print_rotated(buf, root, "");
    return buf.str();
}

template <typename T>
void BinarySearchTree<T>::push(const T& item)
{
    if (!contains(item))
    {
        if (empty())
        {
            root = NodePointer{new Node{item}};
        }
        else
        {
            root = insert_node(root, nullptr, item);
        }
        ++length;
    }
}

template <typename T>
void BinarySearchTree<T>::erase(const T& item)
{
    auto found = locate_node(root, item);
    if (found)
    {
        if (found == root)
        {
            remove_root();
        }
        else
        {
            int n_children = found->children();
            switch (n_children)
            {
                case 0:
                    remove_this_child(found->parent, found);
                    break;
                case 1:
                    remove_internal_node_one_child(found);
                    break;
                case 2:
                    remove_internal_node_two_children(found);
            }
        }
        --length;
    }
    else
    {
        std::ostringstream buf;
        buf << "BinarySearchTree::erase - " << item << " not in tree";
        throw std::runtime_error{buf.str()};
    }
}

template <typename T>
void BinarySearchTree<T>::clear()
{
    root.reset();
    length = 0;
}

template <typename T>
typename BinarySearchTree<T>::NodePointer BinarySearchTree<T>::locate_node(NodePointer current, const T& item) const
{
    while (current)
    {
        if (current->value == item)
        {
            return current;
        }
        current = comparator(item, current->value) ? current->left : current->right;
    }
    return NodePointer{};
}

template <typename T>
typename BinarySearchTree<T>::NodePointer BinarySearchTree<T>::insert_node(NodePointer current,
                                                                           NodePointer current_parent,
                                                                           const T& item)
{
    if (current)
    {
        if (current->value == item)
        {
            // pass; BST holds unique values
        }
        else if (comparator(item, current->value))
        {
            current->left = insert_node(current->left, current, item);
        }
        else
        {
            current->right = insert_node(current->right, current, item);
        }
        return current;
    }
    else
    {
        return NodePointer{new Node{item, nullptr, nullptr, current_parent}};
    }
}

template <typename T>
void BinarySearchTree<T>::print_rotated(std::ostringstream& buf, NodePointer current, const std::string& indent) const
{
    if (current)
    {
        print_rotated(buf, current->right, indent + "..");
        buf << indent << current->value << std::endl;
        print_rotated(buf, current->left, indent + "..");
    }
}

template <typename T>
int BinarySearchTree<T>::height_of(NodePointer current) const
{
    if (current)
    {
        int left = height_of(current->left);
        int right = height_of(current->right);
        return 1 + std::max(left, right);
    }
    else
    {
        return -1;
    }
}

template <typename T>
int BinarySearchTree<T>::calculate_size(NodePointer start) const
{
    if (start)
    {
        return 1 + calculate_size(start->left) + calculate_size(start->right);
    }
    else
    {
        return 0;
    }
}

template <typename T>
int BinarySearchTree<T>::Node::children() const
{
    if (left && right)
    {
        return 2;
    }
    else if (left || right)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

template <typename T>
void BinarySearchTree<T>::remove_internal_node_one_child(BinarySearchTree<T>::NodePointer node)
{
    auto child = node->left ? node->left : node->right;
    auto parent = node->parent;
    if (parent->left == node)
    {
        parent->left.swap(child);
        if (parent->left)
        {
            parent->left->parent = parent;
        }
    }
    else
    {
        parent->right.swap(child);
        if (parent->right)
        {
            parent->right->parent = parent;
        }
    }
    node.reset();
}

template <typename T>
void BinarySearchTree<T>::remove_internal_node_two_children(BinarySearchTree<T>::NodePointer node)
{
    auto minimum_of_right = find_minimum_of(node->right);
    if (minimum_of_right)
    {
        remove_internal_node_of_two_children(node, minimum_of_right);
    }
    else
    {
        auto maximum_of_left = find_maximum_of(node->left);
        remove_internal_node_of_two_children(node, maximum_of_left);
    }
}

template <typename T>
void BinarySearchTree<T>::remove_internal_node_of_two_children(NodePointer node, NodePointer which_child)
{
    T value = which_child->value;
    int children = which_child->children();
    switch (children)
    {
        case 0:
            remove_this_child(which_child->parent, which_child);
            break;
        case 1:
            remove_internal_node_one_child(which_child);
            break;
        default:
            std::ostringstream buf;
            buf << "BinarySearchTree::remove_internal_node_of_two_children - ";
            buf << " removal of node " << node << " with child " << which_child << " has " << children << " children";
            throw std::logic_error{buf.str()};
    }
    node->value = value;
}

template <typename T>
void BinarySearchTree<T>::remove_root()
{
    if (size() == 1)
    {
        root.reset();
    }
    else
    {
        auto minimum_of_right = find_minimum_of(root->right);
        T value;
        if (minimum_of_right)
        {
            value = std::move(minimum_of_right->value);
            remove_internal_node_one_child(minimum_of_right);
        }
        else
        {
            auto maximum_of_left = find_maximum_of(root->left);
            if (maximum_of_left)
            {
                value = std::move(maximum_of_left->value);
                remove_internal_node_one_child(maximum_of_left);
            }
            else
            {
                std::ostringstream buf;
                buf << "BinarySearchTree::remove_root - failed to find smallest larger value, or largest smaller value";
                throw std::logic_error{buf.str()};
            }
        }
        root->value = value;
    }
}

template <typename T>
void BinarySearchTree<T>::remove_this_child(BinarySearchTree<T>::NodePointer parent,
                                            BinarySearchTree<T>::NodePointer child)
{
    if (parent->left == child)
    {
        parent->left.reset();
    }
    else if (parent->right == child)
    {
        parent->right.reset();
    }
    else
    {
        std::ostringstream buf;
        buf << "BinarySearchTree::remove_this_child - node (value: " << child->value << ")";
        buf << " is not a child of parent (value: " << parent->value << ")";
        throw std::logic_error{buf.str()};
    }
}

template <typename T>
typename BinarySearchTree<T>::NodePointer BinarySearchTree<T>::find_minimum_of(BinarySearchTree<T>::NodePointer node)
{
    if (node)
    {
        auto current = node;
        while (current->left)
        {
            current = current->left;
        }
        return current;
    }
    else
    {
        return NodePointer{nullptr};
    }
}

template <typename T>
typename BinarySearchTree<T>::NodePointer BinarySearchTree<T>::find_maximum_of(NodePointer node)
{
    if (node)
    {
        auto current = node;
        while (current->right)
        {
            current = current->right;
        }
        return current;
    }
    else
    {
        return NodePointer{nullptr};
    }
}

template <typename T>
auto BinarySearchTree<T>::begin() const -> const_iterator
{
    return const_iterator{root, nullptr, size()};
}

template <typename T>
auto BinarySearchTree<T>::end() const -> const_iterator
{
    return const_iterator{root, nullptr, size(), size()};
}

template <typename T>
BinarySearchTree<T>::const_iterator::const_iterator(NodePointer start, NodePointer now, int so_far, int length_of)
    : ref{start}, current{now}, traversed{so_far}, cap{length_of}
{
}

template <typename T>
BinarySearchTree<T>::const_iterator::const_iterator(NodePointer start, NodePointer now, int length_of)
    : const_iterator{start, now, 0, length_of}
{
    current = seek_min(start);
    ++traversed;
}

template <typename T>
bool BinarySearchTree<T>::const_iterator::done() const
{
    return traversed >= cap;
}

template <typename T>
typename BinarySearchTree<T>::NodePointer BinarySearchTree<T>::const_iterator::seek_min(BinarySearchTree<T>::NodePointer start)
{
    auto c = start;
    while (c && c->left)
    {
        c = c->left;
    }
    return c;
}

template <typename T>
void BinarySearchTree<T>::const_iterator::advance()
{
    if (traversed == 0)
    {
        current = seek_min(ref);
    }
    else
    {
        auto parent = current->parent;
        if (parent)
        {
            if (current->right)
            {
                last_seen = current;
                current = seek_min(current->right);
            }
            else
            {
                current = parent;
                if (current == last_seen)
                {
                    advance();
                    last_seen = nullptr;
                    return;
                }
            }
        }
        else    // current is the root
        {
            current = seek_min(ref->right);
        }
    }
    ++traversed;
}

template <typename T>
auto BinarySearchTree<T>::const_iterator::operator++() -> const_iterator&
{
    if (!done())
    {
        advance();
    }
    return *this;
}

template <typename T>
auto BinarySearchTree<T>::const_iterator::operator++(int) -> const_iterator
{
}

template <typename T>
bool BinarySearchTree<T>::const_iterator::operator==(const const_iterator& other) const
{
    return ref == other.ref && traversed == other.traversed;
}

template <typename T>
bool BinarySearchTree<T>::const_iterator::operator!=(const const_iterator& other) const
{
    return !operator==(other);
}

template <typename T>
const T& BinarySearchTree<T>::const_iterator::operator*() const
{
    if (done())
    {
        std::ostringstream buf;
        buf << "BinarySearchTree::iterator::operator* - iterator past end";
        throw std::out_of_range{buf.str()};
    }
    else
    {
        return current->value;
    }
}

template <typename T>
const typename BinarySearchTree<T>::NodePointer BinarySearchTree<T>::const_iterator::operator->() const
{
    if (done())
    {
        std::ostringstream buf;
        buf << "BinarySearchTree::iterator::operator-> - iterator past end";
        throw std::out_of_range{buf.str()};
    }
    else
    {
        return current;
    }
}

#endif // DATA_STRUCTURES_BINARY_SEARCH_TREE_HPP
