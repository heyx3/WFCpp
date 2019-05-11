#pragma once

#include "Nullable.hpp"
#include "Array2D.hpp"
#include "List.hpp"
#include "Set.hpp"
#include "Dictionary.hpp"
#include "EnumFlags.h"

#include <random>


namespace WFC
{
    //TODO: Switch to custom PRNG struct.
    using PRNG = std::mt19937;
}