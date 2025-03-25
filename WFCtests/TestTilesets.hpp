#pragma once

#include <UnitTest++.h>

#include <Simple/State.h>
#include <Tiled/State.h>
#include <Tiled3D/StandardRunner.h>
#include <Helpers/WFCppStreamPrinting.hpp>


namespace Tilesets
{
    namespace Simple
    {
        using namespace WFC;
        using namespace WFC::Simple;

        //TODO: Make Simple tests.
    }
    
    namespace Tiled
    {
        using namespace WFC;
        using namespace WFC::Tiled;

        //TODO: Make Tiled tests.
    }

    namespace Tiled3D
    {
        using namespace WFC;
        using namespace WFC::Tiled3D;

        //A single tile, whose opposite faces match up with various symmetry patterns:
        //   * X faces have major-diagonal symmetry
        //         (you can flip the face along its main diagonal).
        //   * Y faces have no symmetries (each point has a different ID).
        //   * Z faces have total symmetry (you can use any transformation).
        template<typename... Transforms>
        inline std::vector<Tile> OneTileArmy(Transforms... transforms)
        {
            auto permutations = TransformSet::Combine(transforms...);
            std::vector<Tile> output;
            output.push_back(WFC::Tiled3D::Tile{ });
            auto& tile = output.back();

            //Remember point ordering: AA, AB, BA, BB
            tile.Data.Faces[0] = { MinX, { { 0, 1, 1, 2 }, { 0, 0, 0, 0 } } };
            tile.Data.Faces[1] = { MaxX, { { 0, 1, 1, 2 }, { 0, 0, 0, 0 } } };
            tile.Data.Faces[2] = { MinY, { { 4, 5, 6, 7 }, { 0, 0, 0, 0 } } };
            tile.Data.Faces[3] = { MaxY, { { 4, 5, 6, 7 }, { 0, 0, 0, 0 } } };
            tile.Data.Faces[4] = { MinZ, { { 3, 3, 3, 3 }, { 0, 0, 0, 0 } } };
            tile.Data.Faces[5] = { MaxZ, { { 3, 3, 3, 3 }, { 0, 0, 0, 0 } } };
            tile.Permutations = permutations;
            tile.Weight = 100;

            return output;
        }

        //A handful of tiles with connecting rods.
        //For full info, see Billy's sketched unit test on the Remarkable.
        namespace SymmetricRods
        {
            //A specific face of a specific tile (before transformation) in the SymmetricRods set.
            //Supports hashing and equality.
            struct SRFace
            {
                TileIdx Tile;
                Directions3D Face;

                bool operator==(SRFace f) const { return Tile == f.Tile && Face == f.Face; }

                uint32_t GetHashcode() const
                {
                    return Vector2i{ Tile, Face }.GetHashcode();
                }

            };
            #pragma region std::hash support
            } } }
            template<> struct std::hash<Tilesets::Tiled3D::SymmetricRods::SRFace> {
                size_t operator()(const Tilesets::Tiled3D::SymmetricRods::SRFace& f) const { return f.GetHashcode(); }
            };
            namespace Tilesets { namespace Tiled3D { namespace SymmetricRods {
            #pragma endregion

            struct SRSet
            {
                //The input tiles.
                std::vector<Tile> Tiles;
                //Which faces match each other (before permutations).
                std::vector<std::unordered_set<SRFace>> FaceGroups;
            };

            template<typename... Transforms>
            inline SRSet Create(Transforms... transforms)
            {
                //For full info, see Billy's sketched unit test on the Remarkable.

                auto permutations = TransformSet::Combine(transforms...);
                SRSet output;

                output.Tiles.emplace_back();
                output.Tiles.emplace_back();
                output.Tiles.emplace_back();

                output.FaceGroups.emplace_back();
                output.FaceGroups.emplace_back();
                output.FaceGroups.emplace_back();
                
                std::vector<FaceIdentifiers> points{
                    { { 1, 1, 1, 1 }, { 0, 0, 0, 0 } },
                    { { 2, 2, 2, 2 }, { 0, 0, 0, 0 } },
                    { { 3, 3, 3, 3 }, { 0, 0, 0, 0 } }
                };

                auto setFace = [&](int tile, Directions3D face, int pointsIdx)
                {
                    output.Tiles[tile].Data.Faces[face] = { face, points[pointsIdx] };
                    output.FaceGroups[pointsIdx].emplace(static_cast<TileIdx>(tile), face);
                };

                setFace(0, Directions3D::MinX, 1);
                setFace(0, Directions3D::MaxX, 1);
                setFace(0, Directions3D::MinY, 0);
                setFace(0, Directions3D::MaxY, 0);
                setFace(0, Directions3D::MinZ, 1);
                setFace(0, Directions3D::MaxZ, 1);
                output.Tiles[0].Permutations = permutations;
                output.Tiles[0].Weight = 100;

                setFace(1, Directions3D::MinX, 1);
                setFace(1, Directions3D::MaxX, 0);
                setFace(1, Directions3D::MinY, 1);
                setFace(1, Directions3D::MaxY, 0);
                setFace(1, Directions3D::MinZ, 0);
                setFace(1, Directions3D::MaxZ, 0);
                output.Tiles[1].Permutations = permutations;
                output.Tiles[1].Weight = 100;

                setFace(2, Directions3D::MinX, 0);
                setFace(2, Directions3D::MaxX, 0);
                setFace(2, Directions3D::MinY, 0);
                setFace(2, Directions3D::MaxY, 0);
                setFace(2, Directions3D::MinZ, 2);
                setFace(2, Directions3D::MaxZ, 1);
                output.Tiles[2].Permutations = permutations;
                //This tile is useful, so use a low weight to ensure clearing happens more often.
                output.Tiles[2].Weight = 20;

                return output;
            }
        }

        //TODO: 2 tiles that line up with each other in strange face pairings
    }
}