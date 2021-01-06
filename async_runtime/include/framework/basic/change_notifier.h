#pragma once

#include "listenable.h"

class ChangeNotifier : public virtual Listenable
{
public:
    virtual void notifyListeners();
};