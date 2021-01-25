#include "../object.h"

class Error : public Object
{
public:
    Error(String &&message) : message(std::forward<String>(message)) {}
    Error(String &&message, Object::Ref<Error> from) : from(from), message(std::forward<String>(message)) {}
    Error(const Error &other) : from(other.from), message(other.message) {}

    const Object::Ref<Error> from;
    const String message;

    void toStringStream(std::ostream &os) override;
};