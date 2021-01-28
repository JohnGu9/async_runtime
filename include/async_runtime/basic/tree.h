#pragma once

#include "../object.h"

class Tree : public Object
{
public:
    Object::WeakRef<Tree> parent;
    List<Object::Ref<Tree>> children;
    String info;

    virtual void ps(std::ostream &ss)
    {
        if (info.isEmpty())
            ss << '[' << this->runtimeType() << ']' << std::endl;
        else
            ss << info << std::endl;
    }

    void toStringStream(std::ostream &ss) override
    {
        std::stringstream childrenStringStream;
        for (auto child : this->children)
            child->toStringStream(childrenStringStream);
        ps(ss);

        std::string line;
        while (std::getline(childrenStringStream, line))
            ss << '\t' << line << std::endl;
    }
};