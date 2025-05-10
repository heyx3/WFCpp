#pragma once

#include <optional>
#include <vector>
#include "Helpers/Array2D.hpp"
#include "Helpers/Array3D.hpp"
#include "Helpers/Array4D.hpp"
#include <unordered_set>
#include <unordered_map>
#include "Helpers/EnumFlags.h"
#include "Helpers/xoshiro.hpp"

#include <random>


namespace WFC
{
    //Keep in mind WFC only samples the RNG like twice per iteration.
    //We don't need something really tight like PCG.
    using PRNG = xoshiro256starstar64;


    //Unfortunately, std::discrete_distribution is really slow to create,
    //     and its allocations seemingly can't be re-used,
    //     so below is my own implementation.

    //Returns a random index from the 'weights' list, using its elements as random weights.
    //Returns -1 if there are no elements or your weights don't sum to a positive number.
    template<typename Weight, typename Rng, typename WeightsAllocator>
    ptrdiff_t PickWeightedRandomIndex(Rng& rng,
                                      std::vector<Weight, WeightsAllocator>& weights,
                                      Weight cachedTotalWeight = -1)
    {
        if (weights.size() < 1)
            return -1;

        //Generate a random float from 0 to the total weight, then use that as a "budget"
        //    to search for the last element before that budget is exhausted.
        //There are multiple faster algorithms to do this,
        //    but they're all harder to implement and I don't think this is a bottleneck.

        Weight totalWeight;
        if (cachedTotalWeight > Weight{ 0 })
            totalWeight = cachedTotalWeight;
        else
        {
            totalWeight = { 0 };
            for (const Weight& w : weights)
                totalWeight += w;

            if (totalWeight <= Weight{ 0 })
                return -1;
        }

        Weight remainingBudget = std::uniform_real_distribution<Weight>(0, totalWeight)(rng);
        //Due to floating-point error we may run off the end of the weights array;
        //    plus once we pass the second-to-last element we already know what the answer will be.
        for (int i = 0; i < static_cast<int>(weights.size()) - 1; ++i)
        {
            remainingBudget -= weights[i];
            if (remainingBudget <= 0)
                return i;
        }
        return static_cast<ptrdiff_t>(weights.size()) - 1;
    }
}