#ifndef PROJECT3_MEMORY_EXCEPTION_HPP
#define PROJECT3_MEMORY_EXCEPTION_HPP

#include <exception>
#include <string>


class MemoryException : public std::exception
{
public:
    explicit MemoryException(const std::string& msg);
    MemoryException();

    virtual const char* what() const noexcept override;

private:
    std::string message;
};

#endif //PROJECT3_MEMORY_EXCEPTION_HPP
