#include "../../include/Tiled3D/TilePermutator.h"
#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


namespace
{
    template<typename T>
    bool Contains(const std::vector<T>& list, T t)
    {
        for (const auto& t2 : list)
            if (t == t2)
                return true;
        return false;
    }
}


TilePermutator::TilePermutator()
    : TilePermutator(std::vector<Tile>(), std::vector<std::vector<Transform3D>>(), std::vector<Transform3D>())
{
    
}
TilePermutator::TilePermutator(const std::vector<Tile>& originalTiles,
                               const std::vector<std::vector<Transform3D>>& tileSymmetries,
                               const std::vector<Transform3D>& permutationsToUse)
    : tiles(originalTiles), nOriginalTiles(originalTiles.size())
{
    //Reserve space in various collections.
    permutationsPerParent.reserve(nOriginalTiles);
    tileParents.reserve(nOriginalTiles * permutationsToUse.size());

    //Fill in the original tiles' "parent" data to point to themselves.
    for (TileID parentID = 0; parentID < (TileID)originalTiles.size(); ++parentID)
    {
        tileParents.push_back(ParentData{ parentID, Transform3D() });
        permutationsPerParent[parentID].push_back(parentID);
    }

    //Make each permutation.
    WFCPP_ASSERT(!Contains(permutationsToUse, Transform3D()));
    for (auto permutation : permutationsToUse)
    {
        for (TileID parentID = 0; parentID < (TileID)nOriginalTiles; ++parentID)
        {
            //If this tile is symmetric under this permutation, don't make a duplicate.
            if (Contains(tileSymmetries[parentID], permutation))
                continue;

            //Create the permutation.
            Tile childTile = originalTiles[parentID];
            childTile.Data = permutation.ApplyToCube(childTile.Data);

            //Register the permutation.
            TileID childID = (TileID)tiles.size();
            tiles.push_back(childTile);
            tileParents.push_back(ParentData{ parentID, permutation });
            permutationsPerParent[parentID].push_back(childID);
        }
    }
}

TileID TilePermutator::FindPermutation(TileID original, Transform3D permutation) const
{
    for (auto childID : permutationsPerParent.at(original))
        if (tileParents[childID].MyTransform == permutation)
            return childID;

    return TileID_INVALID;
}