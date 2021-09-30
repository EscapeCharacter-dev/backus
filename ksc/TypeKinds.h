#ifndef KSC_TYPE_KINDS_H
#define KSC_TYPE_KINDS_H

#include "Types.h"

enum
{
    // void type
    KSC_TYPE_VOID,
    // group of types (usually passed around by multiple value storage)
    KSC_TYPE_TUPLE,
    // structure, passed by stack
    KSC_TYPE_STRUCT,
    // constant member structure, passed by stack
    KSC_TYPE_RECORD,
    // union, passed by stack
    KSC_TYPE_UNION,
    // enum, passed by register
    KSC_TYPE_ENUM,
    // pointer, passed by register
    KSC_TYPE_POINTER,
    // array, passed by register (pointer)
    KSC_TYPE_ARRAY,
    // reference, passed by register (safe pointer)
    KSC_TYPE_REFERENCE,
    // function pointer, passed by register
    KSC_TYPE_FUNCTIONPTR,

    // 8-bit signed value
    KSC_TYPE_SBYTE,
    // 8-bit unsigned value
    KSC_TYPE_BYTE,

    // 16-bit signed integer value
    KSC_TYPE_SHORT,
    // 16-bit unsigned integer value
    KSC_TYPE_USHORT,
    // 16-bit floating value
    KSC_TYPE_HALF,

    // 32-bit signed integer value
    KSC_TYPE_INT,
    // 32-bit unsigned integer value
    KSC_TYPE_UINT,
    // 32-bit floating value
    KSC_TYPE_FLOAT,

    // 64-bit signed integer value
    KSC_TYPE_LONG,
    // 64-bit unsigned integer value
    KSC_TYPE_ULONG,
    // 64-bit floating value
    KSC_TYPE_DOUBLE,
};

#endif