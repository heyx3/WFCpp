#include "TilePermutator.h"
#include <algorithm>

using namespace WFC;
using namespace WFC::Math;
using namespace WFC::Tiled3D;


namespace
{
    template<typename T>
    bool Contains(const List<T>& list, T t)
    {
        for (const auto& t2 : list)
            if (t == t2)
                return true;
        return false;
    }
}


TilePermutator::TilePermutator()
    : TilePermutator(List<Tile>(), List<List<Transform3D>>(), List<Transform3D>())
{
    
}
TilePermutator::TilePermutator(const List<Tile>& originalTiles,
                               const List<List<Transform3D>>& tileSymmetries,
                               const List<Transform3D>& permutationsToUse)
    : tiles(originalTiles), nOriginalTiles(originalTiles.GetSize())
{
    //Reserve space in various collections.
    permutationsPerParent.Reserve(nOriginalTiles);
    tileParents.Reserve(nOriginalTiles * permutationsToUse.GetSize());

    //Fill in the original tiles' "parent" data to point to themselves.
    for (TileID parentID = 0; parentID < (TileID)originalTiles.GetSize(); ++parentID)
    {
        tileParents.PushBack(ParentData{ parentID, Transform3D() });
        permutationsPerParent[parentID].PushBack(parentID);
    }

    //Make each permutation.
    assert(!Contains(permutationsToUse, Transform3D()));
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
            TileID childID = (TileID)tiles.GetSize();
            tiles.PushBack(childTile);
            tileParents.PushBack(ParentData{ parentID, permutation });
            permutationsPerParent[parentID].PushBack(childID);
        }
    }
}

TileID TilePermutator::FindPermutation(TileID original, Transform3D permutation) const
{
    for (auto childID : permutationsPerParent[original])
        if (tileParents[childID].MyTransform == permutation)
            return childID;

    return TileID_INVALID;
}