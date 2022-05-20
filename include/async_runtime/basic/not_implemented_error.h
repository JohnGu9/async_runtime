#include <exception>

class NotImplementedError : public std::exception
{
public:
    NotImplementedError() noexcept : std::exception(){};
    const char *what() const noexcept override { return "Function not yet implemented"; }
};
