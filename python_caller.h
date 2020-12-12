#ifndef PYTHON_CALLER_H
#define PYTHON_CALLER_H

#define PY_SSIZE_T_CLEAN
#pragma push_macro("slots")
#undef slots
#include "Python.h"
#pragma pop_macro("slots")
#include <QCoreApplication>
#include <QJsonValue>

#include <string>

class python_caller
{
public:
    python_caller();

    int callFuncFromModule(int argc, char * argv[]);
};

#endif // PYTHON_CALLER_H
