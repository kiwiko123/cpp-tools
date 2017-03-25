#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "path.hpp"



namespace pathlib {

Path::Path(const std::string& the_path, char separator)
	: sep{separator}
{
	set_name_attributes(the_path);
	stat(_path.c_str(), &st);
}

Path::Path(const Path& other)
	: _path{other._path}, _name{other._name}, _extension{other._extension},
	  _parent{other._parent}, sep{other.sep}
{
	set_name_attributes(other._path);
	stat(_path.c_str(), &st);
}

Path& Path::operator=(const Path& other)
{
	if (this != &other)
	{
		_path = other._path;
		_name = other._name;
		_extension = other._extension;
		_parent = other._parent;
		sep = other.sep;

		set_name_attributes(other._path);
		stat(_path.c_str(), &st);
	}
	return *this;
}

bool Path::operator==(const Path& other) const
{
	return _path == other._path;
}

bool Path::operator!=(const Path& other) const
{
	return !operator==(other);
}

std::ostream& operator<<(std::ostream& os, const Path& p)
{
	os << "Path(\"" << p._path << "\")";
	return os;
}

bool Path::is_file() const
{
	return S_ISREG(st.st_mode);
}

bool Path::is_dir() const
{
	return S_ISDIR(st.st_mode);
}

bool Path::exists() const
{
	return is_dir() || is_file();
}

const std::string& Path::path() const
{
	return _path;
}

const std::string& Path::name() const
{
	ensure_existence("pathlib::Path::get_name");
	return _name;
}

std::string Path::extension() const
{
	return _extension;
}

const std::string& Path::parent() const
{
	ensure_existence("pathlib::Path::get_parent");
	return _parent;
}

std::vector<Path> Path::collectdir() const
{
	ensure_dir("pathlib::Path::collect");
	std::vector<Path> result;
	DIR* dir = opendir(_path.c_str());
	if (dir == NULL)
		throw PathError{"Error opening path: " + _path};
	struct dirent* entry;

	while ((entry = readdir(dir)) != NULL)
	{
		if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
		{
			std::ostringstream fmt;
			fmt << _path << sep << entry->d_name;
			result.push_back(Path{fmt.str(), sep});
		}
	}

	closedir(dir);
	return result;
}

void Path::listdir() const
{
	for (const auto& entry : collectdir())
		std::cout << entry.path() << std::endl;
}

std::ifstream Path::open() const
{
	ensure_file("pathlib::Path::open");
	return std::ifstream{_path, std::ifstream::in};
}


/// <iterator implementation>
auto Path::begin() const -> Path::iterator
{
	ensure_dir("Path::begin()");
	return iterator{const_cast<Path*>(this), opendir(_path.c_str())};
}

auto Path::end() const -> Path::iterator
{
	return iterator{const_cast<Path*>(this), NULL};
}

Path::iterator::iterator(Path* p, DIR* opened)
	: ref{p}, opened_dir{opened}
{
	ref->ensure_dir("Path::iterator::iterator()");
	if (opened_dir != NULL)
		current = readdir(opened_dir);
}

Path::iterator::~iterator()
{
	if (opened_dir != NULL)
		closedir(opened_dir);
}

auto Path::iterator::operator++() -> Path::iterator&
{
	if (opened_dir != NULL && (current = readdir(opened_dir)) != NULL)
	{
		if (strcmp(current->d_name, ".") == 0 || strcmp(current->d_name, "..") == 0)
			operator++();
	}
	return *this;
}

auto Path::iterator::operator++(int) -> Path::iterator
{
	iterator state{*this};
	operator++();
	return state;
}

bool Path::iterator::operator==(const Path::iterator& other) const
{
	bool current_flag = current == NULL || other.current == NULL ? current == other.current : strcmp(current->d_name, other.current->d_name) == 0;
	return ref == other.ref && current_flag;
}

bool Path::iterator::operator!=(const Path::iterator& other) const
{
	return !operator==(other);
}

Path Path::iterator::operator*() const
{
	if (opened_dir == NULL || current == NULL)
		throw PathError{"Path::iterator::operator* - current position invalid; iterator already past end"};
	return Path{ref->parent() + current->d_name};
}

struct dirent* const Path::iterator::operator->() const
{
	if (opened_dir == NULL || current == NULL)
		throw PathError{"Path::iterator::operator* - current position invalid; iterator already past end"};
	return current;
}
/// </iterator implementation>


/// <private member functions>
void Path::ensure_existence(const std::string& function_name) const
{
	if (!exists())
		throw PathError{function_name + " -- path does not exist:\n " + _path};
}

void Path::ensure_dir(const std::string& function_name) const
{
	if (!is_dir())
		throw PathError{function_name + " -- path does not point to a valid directory:\n" + _path};
}

void Path::ensure_file(const std::string& function_name) const
{
	if (!is_file())
		throw PathError{function_name + " -- path does not point to a valid file:\n " + _path};
}

char* Path::c_canonize_pathname(char* pathname)
{
	unsigned int length = strlen(pathname) - 1;
	if (pathname[length] == sep)
		pathname[length] = '\0';

	return pathname;
}

void Path::set_name_attributes(const std::string& pathname)
{
	char* buf = new char[pathname.size() + 1];
	strcpy(buf, pathname.c_str());
	_path = std::string{c_canonize_pathname(buf)};
	char* pch = strchr(buf, sep);

	while (pch != NULL)
	{
		char* next = strchr(pch + 1, sep);
		if (next == NULL)
			break;
		else
			pch = next;
	}

	// remove leading '/' from file name
	if (pch[0] == sep)
		++pch;
	_name = std::string{pch};

	// get extension from file name
	char* ext = strchr(pch, '.');
	_extension = ext == NULL ? "" : std::string{ext};

	// remove leading '/' from parent path name;
	// get parent path
	pch[0] = '\0';
	_parent = std::string{buf};

	delete[] buf;
}
/// </private member functions>


/// <PathError implementation>
Path::PathError::PathError(const std::string& message)
	: reason{message}
{
}

Path::PathError::PathError() : PathError{""}
{
}

const char* Path::PathError::what() const noexcept
{
	return reason.c_str();
}
/// </PathError implementation>


}
