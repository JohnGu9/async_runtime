#pragma once

#include "../object.h"

class Tree : public Object
{
public:
    Tree() : children(Object::create<List<ref<Tree>>>()) {}
    weakref<Tree> parent;
    ref<List<ref<Tree>>> children;
    option<String> info;

    virtual void ps(std::ostream &ss)
    {
        lateref<String> in;
        if (info.isNotNull(in) && in->isNotEmpty())
            ss << in << std::endl;
        else
            ss << '[' << this->runtimeType() << ']' << std::endl;
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