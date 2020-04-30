#include <exception>
#include <iostream>
#include <string>

class Exception : public std::exception 
{
    std::string msg_;

public:
    Exception(const std::string& msg) : msg_(msg) {}

    virtual const char* what() const noexcept override
    {
        return msg_.c_str();
    }
};
