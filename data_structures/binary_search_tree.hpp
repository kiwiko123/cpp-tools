// This header file defines a Binary Search Tree (BST) class.
// Lookup, insertion, and removal operations are done in Θ(logN) time
// (or, more accurately, O(height) time).
//
// Public member functions provide a high-level interface for using this class
// as the underlying data structure implementation for a data type such as an ordered map or set.
//
// A BST holds objects of the templated type 'T';
// it uses T::operator< for internal ordering and comparisons.
// If type T implements no operator<, operator>, and operator==, BST will fail.
//
// Author: Geoffrey Ko (2016)
// Additional Credits: Professor Richard Pattis -- University of California, Irvine -- ICS 46: Data Structure Implementation & Analysis
#ifndef BINARY_SEARCH_TREE_HPP
#define BINARY_SEARCH_TREE_HPP

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stack>
#include <stdexcept>
#include <string>


template <typename T>
class BST
{
public:
	BST();
	BST(const BST<T>& other);

	template <typename Container>
	explicit BST(const Container& other);

	virtual ~BST();

	// --- Operators ---
	template <typename T2>
	friend std::ostream& operator<<(std::ostream& os, const BST<T2>& tree);
	virtual bool operator==(const BST<T>& other) const;
	virtual bool operator!=(const BST<T>& other) const;
	BST<T>& operator=(const BST<T>& other);

	// --- Non-Modifying Member Functions ---
	std::string str() const;
	int size() const;
	bool empty() const;
	int height() const;
	int depth_of(const T& value) const;
	bool contains(const T& value) const;
	const T& top() const;

	// --- Modifying Member Functions ---
	virtual void insert(const T& value);
	virtual void erase(const T& value);
	void clear();


protected:
	struct node
	{
	public:
		T value;
		node* parent = nullptr;
		node* left = nullptr;
		node* right = nullptr;
	};

	node* root;
	int length;

	static node* _insert(node*& start, node* parent, const T& value);
	static int _height(node* start);

	node*& find(const T& value);


private:
	static int _depth(node* start, const T& value, int current_depth);
	static void delete_tree(node*& start);
	T _remove_closest(node*& start);
	void _remove(node*& start, const T& value);
	node* _copy(node* start);

	template <typename stream>
	static void print_rotated(node* start, stream& os, const std::string& indent);


public:
	// --- iterator ---
	class iterator;
	auto begin() const -> iterator;
	auto end() const -> iterator;

	class iterator : public std::iterator<std::forward_iterator_tag, T>
	{
	public:
		auto operator++() -> iterator&;
		auto operator++(int) -> iterator;
		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;
		T& operator*() const;
		T* operator->() const;

		friend iterator BST<T>::begin() const;
		friend iterator BST<T>::end() const;

	private:
		iterator(BST<T>* tree, node* start);
		void push_left(node* start);

		BST<T>* ref;
		std::stack<node*> unvisited;
	};
};


template <typename T>
BST<T>::BST()
	: root{nullptr}, length{0}
{
}

template <typename T>
BST<T>::BST(const BST<T>& other) : BST{}
{
	for (const auto& item : other)
		insert(item);
}

template <typename T>
template <typename Container>
BST<T>::BST(const Container& other) : BST{}
{
	for (const auto& item : other)
		insert(item);
}

template <typename T>
BST<T>::~BST()
{
	delete_tree(root);
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const BST<T>& tree)
{
	os << "BST(";
	if (!tree.empty())
	{
		auto i = tree.begin();
		os << *(i++);
		for (; i != tree.end(); ++i)
			os << ", " << *i;
	}
	os << ")";
	return os;
}

template <typename T>
bool BST<T>::operator==(const BST<T>& other) const
{
	if (this == &other)
		return true;
	if (size() != other.size())
		return false;

	for (auto left = begin(), right = other.begin(); left != end(); ++left, ++right)
	{
		if (*left != *right)
			return false;
	}
	return true;
}

template <typename T>
bool BST<T>::operator!=(const BST<T>& other) const
{
	return !operator==(other);
}

template <typename T>
BST<T>& BST<T>::operator=(const BST<T>& other)
{
	if (this != &other)
	{
		clear();
		for (const auto& item : other)
			insert(item);
	}
	return *this;
}

template <typename T>
std::string BST<T>::str() const
{
	std::ostringstream result;
	print_rotated(root, result, "");
	return result.str();
}

template <typename T>
int BST<T>::size() const
{
	return length;
}

template <typename T>
bool BST<T>::empty() const
{
	return size() == 0;
}

template <typename T>
int BST<T>::height() const
{
	return _height(root);
}

template <typename T>
int BST<T>::depth_of(const T& value) const
{
	return _depth(root, value, 0);
}

template <typename T>
bool BST<T>::contains(const T& value) const
{
	node* current = root;
	while (current != nullptr)
	{
		if (current->value == value)
			return true;
		current = value < current->value ? current->left : current->right;
	}
	return false;
}

template <typename T>
const T& BST<T>::top() const
{
	return root->value;
}

template <typename T>
void BST<T>::insert(const T& value)
{
	if (_insert(root, nullptr, value) != nullptr)
		++length;
}

template <typename T>
void BST<T>::erase(const T& value)
{
	if (contains(value))
	{
		_remove(root, value);
		--length;
	}
	else
		throw std::out_of_range{"BST::erase - does not contain item"};
}

template <typename T>
void BST<T>::clear()
{
	if (!empty())
	{
		delete_tree(root);
		root = nullptr;
	}
}

template <typename T>
auto BST<T>::begin() const -> BST<T>::iterator
{
	return iterator{const_cast<BST<T>*>(this), root};
}

template <typename T>
auto BST<T>::end() const -> BST<T>::iterator
{
	return iterator{const_cast<BST<T>*>(this), nullptr};
}

template <typename T>
BST<T>::iterator::iterator(BST<T>* tree, node* start)
	: ref{tree}
{
	push_left(start);
}

template <typename T>
void BST<T>::iterator::push_left(node* start)
{
	while (start != nullptr)
	{
		unvisited.push(start);
		start = start->left;
	}
}

template <typename T>
auto BST<T>::iterator::operator++() -> BST<T>::iterator&
{
	if (*this != ref->end())
	{
		node* current = unvisited.top();
		unvisited.pop();
		if (current->right != nullptr)
		{
			unvisited.push(current->right);
			push_left(current->right->left);
		}
	}

	return *this;
}

template <typename T>
auto BST<T>::iterator::operator++(int) -> BST<T>::iterator
{
	iterator state{*this};
	operator++();

	return state;
}

template <typename T>
bool BST<T>::iterator::operator==(const BST<T>::iterator& other) const
{
	int sz = unvisited.size();
	int osz = other.unvisited.size();
	if (sz == 0 || osz == 0)
		return sz == osz;
	else
		return sz == osz && unvisited.top()->value == other.unvisited.top()->value;
}

template <typename T>
bool BST<T>::iterator::operator!=(const BST<T>::iterator& other) const
{
	return !operator==(other);
}

template <typename T>
T& BST<T>::iterator::operator*() const
{
	if (unvisited.empty())
		throw std::out_of_range{"BST::iterator::operator* - iterator passed end"};
	return unvisited.top()->value;
}

template <typename T>
T* BST<T>::iterator::operator->() const
{
	if (unvisited.empty())
		throw std::out_of_range{"BST::iterator::operator-> - iterator passed end"};
	return unvisited.top();
}

template <typename T>
void BST<T>::delete_tree(node*& start)
{
	if (start == nullptr)
		return;

	delete_tree(start->left);
	delete_tree(start->right);
	delete start;
}

template <typename T>
typename BST<T>::node* BST<T>::_insert(node*& start, node* parent, const T& value)
{
	if (start == nullptr)
	{
		return (start = new node{value, parent});
	}
	else
	{
		if (value < start->value)
			return _insert(start->left, start, value);
		else if (value > start->value)
			return _insert(start->right, start, value);
		else
			return nullptr;
	}
}

template <typename T>
T BST<T>::_remove_closest(node*& start)
{
	if (start->right == nullptr)
	{
		T value = start->value;
		node* previous = start;
		start = start->left;

		delete previous;
		return value;
	}
	else
		return _remove_closest(start->right);
}

template <typename T>
void BST<T>::_remove(node*& start, const T& value)
{
	if (start == nullptr)
		return;
	else if (start->value == value)
	{
		if (start->left == nullptr || start->right == nullptr)
		{
			node* previous = start;
			start = start->left == nullptr ? start->right : start->left;
			delete previous;
		}
		else
			start->value = _remove_closest(start->left);
	}
	else
		_remove(value < start->value ? start->left : start->right, value);
}

template <typename T>
int BST<T>::_height(node* start)
{
	if (start == nullptr)
		return -1;
	return 1 + std::max(_height(start->left), _height(start->right));
}

template <typename T>
int BST<T>::_depth(node* start, const T& value, int current_depth)
{
	if (start == nullptr)
		return -1;
	else if (start->value == value)
		return current_depth;
	else
	{
		++current_depth;
		int left_depth = _depth(start->left, value, current_depth);
		return left_depth == -1 ? _depth(start->right, value, current_depth) : left_depth;
	}
}

template <typename T>
typename BST<T>::node*& BST<T>::find(const T& value)
{
	node* result = root;
	while (result != nullptr && result->value != value)
	{
		result = value < result->value ? result->left : result->right;
	}
	return result;
}

template <typename T>
template <typename stream>
void BST<T>::print_rotated(node* start, stream& os, const std::string& indent)
{
	if (start == nullptr)
		return;

	print_rotated(start->right, os, indent + "..");
	os << indent << start->value << std::endl;
	print_rotated(start->left, os, indent + "..");
}


#endif /* BINARY_SEARCH_TREE_HPP */
