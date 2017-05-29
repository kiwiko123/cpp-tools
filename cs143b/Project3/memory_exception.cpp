#include "memory_exception.hpp"


MemoryException::MemoryException(const std::string& msg)
    : message{msg}
{
}

MemoryException::MemoryException()
    : MemoryException{""}
{
}

const char* MemoryException::what() const noexcept
{
    return message.c_str();
}