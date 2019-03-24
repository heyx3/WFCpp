#pragma once

#include "Nullable.hpp"
#include "Array2D.hpp"
#include "List.hpp"
#include "Set.hpp"
#include "Dictionary.hpp"

#include <random>

//TODO: Support for seeding initial values in both generator types.

namespace WFC
{
    //TODO: Switch to custom PRNG struct.
    using PRNG = std::mt19937;
}