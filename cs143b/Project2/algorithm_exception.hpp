#ifndef PROJECT2_ALGORITHM_EXCEPTION_HPP
#define PROJECT2_ALGORITHM_EXCEPTION_HPP

#include <exception>
#include <string>


class AlgorithmException : public std::exception
{
public:
    AlgorithmException(const std::string& msg);
    AlgorithmException();

    virtual const char* what() const noexcept;


private:
    std::string message;
};

#endif //PROJECT2_ALGORITHM_EXCEPTION_HPP
