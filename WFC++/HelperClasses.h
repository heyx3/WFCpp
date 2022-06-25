#pragma once

#include "Nullable.hpp"
#include "Array2D.hpp"
#include "Array3D.hpp"
#include "Array4D.hpp"
#include "List.hpp"
#include "Set.hpp"
#include "Dictionary.hpp"
#include "EnumFlags.h"

#include <random>


namespace WFC
{
    //We could use a custom RNG that's much more lightweight,
    //    but WFC only samples the RNG like twice per iteration.
    using PRNG = std::mt19937;
}