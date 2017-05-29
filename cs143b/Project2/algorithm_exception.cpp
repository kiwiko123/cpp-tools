#include "algorithm_exception.hpp"


AlgorithmException::AlgorithmException(const std::string& msg)
    : message{msg}
{
}

AlgorithmException::AlgorithmException()
    : AlgorithmException{""}
{
}

const char* AlgorithmException::what() const noexcept
{
    return message.c_str();
}