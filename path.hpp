// This header file defines a Path object, based off of Python's pathlib.Path class.
//
// It is implemented using the POSIX library's <dirent.h> header --
// as such, it will only function correctly on UNIX environments.
// It is essentially a modern C++ wrapper around a C/POSIX library.
//
// **Developed and tested using clang-703.0.31**
//
// author: Geoffrey Ko (2016)
#ifndef PATHLIB_PATH_HPP
#define PATHLIB_PATH_HPP

#include <exception>
#include <forward_list>
#include <fstream>
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>


namespace pathlib {

class Path
{
public:
	explicit Path(const std::string& the_path, char separator = '/');

	// Operators
	friend std::ostream& operator<<(std::ostream& os, const Path& p);
	bool operator==(const Path& other) const;
	bool operator!=(const Path& other) const;

	// Member Functions
	/* Returns true if this path points to a file */
	bool is_file() const;

	/* Returns true if this path points to a directory */
	bool is_dir() const;

	/* Returns true if this path points to a valid file or directory */
	bool exists() const;

	/* Returns the full path; e.g., the string it was contructed with */
	const std::string& get_path() const;

	/* Returns the name of the path only */
	const std::string& get_name() const;

	/* Returns the name of the extension of path, or empty string if there is none */
	std::string get_extension() const;

	/* Returns the full parent directory of this path;
	 * e.g., the directory that this lies in.
	 */
	const std::string& get_parent() const;

	/* Returns a std::forward_list of Path objects consisting of all items directly in this Path.
	 * this must be a directory.
	 */
	std::forward_list<Path> iterdir() const;

	/* Prints every item directly in this Path, one per line.
	 * this must be a directory.
	 */
	void listdir() const;

	/* Returns an opened file object that Path points to.
	 * If Path does not point to a file, throws PathError.
	 */
	std::ifstream open() const;


	/* Path throws PathError exception */
	class PathError : public std::exception
	{
	public:
		PathError();
		PathError(const std::string& message);
		virtual const char* what() const noexcept;

	private:
		std::string reason;
	};


	/* iterator (directory only) */
	class iterator;
	auto begin() const -> iterator;
	auto end() const -> iterator;

	class iterator
	{
	public:
		iterator();
		~iterator();
		auto operator++() -> iterator&;
		auto operator++(int) -> iterator;
		bool operator==(const iterator& other) const;
		bool operator!=(const iterator& other) const;
		Path operator*() const;
		Path* operator->() const;		// currently unimplemented

		friend iterator Path::begin() const;
		friend iterator Path::end() const;

	private:
		iterator(Path* p, DIR* opened);

		Path* ref;
		DIR* opened_dir;
		struct dirent* current;
	};


private:
	std::string path;
	std::string name;
	std::string extension;
	std::string parent;
	char sep;
	struct stat st;

	void ensure_existence(const std::string& function_name) const;
	void ensure_dir(const std::string& function_name) const;
	void ensure_file(const std::string& function_name) const;
	char* c_canonize_pathname(char* pathname);
	void set_name_attributes(const std::string& pathname);
};

}

#endif // PATHLIB_PATH_HPP
