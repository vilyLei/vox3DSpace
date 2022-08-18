#pragma once
#include <iostream>

typedef unsigned char  Uint8;
typedef signed char    Int8;
typedef unsigned short Uint16;
typedef signed short   Int16;
typedef unsigned int   Uint32;
typedef int            Int32;
typedef float          Float32;


#define DELETE_OBJ_NAKED_PTR(objPtr) \
    if (objPtr != nullptr)           \
    {                                \
        delete objPtr;               \
        objPtr = nullptr;            \
    }

#define DELETE_OBJ_ARRAY_NAKED_PTR(objArrPtr) \
    if (objArrPtr != nullptr)                 \
    {                                         \
        delete[] objArrPtr;                   \
        objArrPtr = nullptr;                  \
    }