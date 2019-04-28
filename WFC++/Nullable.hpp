#pragma once

#include "Platform.h"

namespace WFC
{
    template<typename T>
    struct Nullable
    {
    public:
	    bool HasValue;
	    T Value;

	    Nullable() : HasValue(false), Value() { }
	    Nullable(const T& value) : HasValue(true), Value(value) { }

	    Nullable(const Nullable<T>& copy) = default;
	    Nullable(Nullable<T>&& moveFrom) = default;
	    Nullable& operator=(const Nullable<T>& copy) = default;
	    Nullable& operator=(Nullable<T>&& moveFrom) = default;
    };
}