#pragma once

#include "../Platform.h"
#include "../Vector3i.h"

namespace WFC
{
    namespace Tiled3D
    {
        //Unique identifiers for each tile cube.
        //Note that this ID changes under transformation.
        using TileID = uint_fast32_t;

        /*
        //Unique identifiers for tile faces.
        //Note that this ID does not change under transformation (e.x. the face getting mirrored).
        using FaceID = uint_fast32_t;
        */

        //Unique identifiers for points on a tile face.
        //Each type of face should have its own unique PointIDs --
        //    anywhere from 1 to 4, depending on the face's symmetry.
        //Four of these, along with the FaceID, can uniquely identify a face permutation.
        using PointID = uint_fast32_t;


        //Note that we're assuming a left-handed coordinate system.
        //Conceptually, we visualize X as rightward, Z as forward, and Y as upwards.
        //This matches the coordinate system used by the Unity3D game engine.


        //The four points on a face, enumerated based on world coordinates
        //    to remove ambiguity.
        //This makes it easier to see if two faces will line up against each other.
        enum WFC_API FacePoints
        {
            //Specified as A for "min" or B for "max", for the two face axes.
            //The axes are ordered X, Y, Z (e.x. the Y face's axes are ordered "XZ").
            AA, AB, BA, BB
        };
        inline bool WFC_API IsFirstMin(FacePoints p) { return (uint_fast8_t)p / 2 == 0; }
        inline bool WFC_API IsSecondMin(FacePoints p) { return (uint_fast8_t)p % 2 == 0; }
        inline FacePoints WFC_API MakeFacePoint(bool axis1IsMin, bool axis2IsMin)
        {
            return axis1IsMin ?
                       (axis2IsMin ? FacePoints::AA : FacePoints::AB) :
                       (axis2IsMin ? FacePoints::BA : FacePoints::BB);
        }

        //The different axis directions/faces of a cube.
        enum WFC_API Directions3D : uint8_t
        {
            //Note that some functions and lookup tables depend on the exact ordering here!

            MinX, MaxX,
            MinY, MaxY,
            MinZ, MaxZ,

            #define N_DIRECTIONS3D 6
        };
        inline bool WFC_API IsMin(Directions3D dir) { return (uint_fast8_t)dir % 2 == 0; }
        inline bool WFC_API IsMax(Directions3D dir) { return (uint_fast8_t)dir % 2 == 1; }
        inline uint_fast8_t GetAxisIndex(Directions3D dir) { return (uint_fast8_t)dir / 2; }
        inline Directions3D WFC_API GetOpposite(Directions3D dir) { return (Directions3D)(IsMin(dir) ? (dir + 1) : (dir - 1)); }



        //A specific permutation of a face.
        struct WFC_API FacePermutation
        {
            //The identifier for this face.
            //Does not change under permutation.
            //FaceID ID;

            //The four points of this face. Indexed with the "FacePoints" enum.
            PointID Points[4];

            //The side of the cube this face is on.
            Directions3D Side;
        };


        //Different ways to rotate a cube while keeping it axis-aligned.
        enum WFC_API Rotations3D : uint8_t
        {
            //Note that some functions and lookup tables depend on the exact ordering here!

            //0 or 360-degree rotation.
            None,

            //Rotation along an axis, clockwise when facing the positive face.
            AxisX_90, AxisX_180, AxisX_270,
            AxisY_90, AxisY_180, AxisY_270,
            AxisZ_90, AxisZ_180, AxisZ_270,

            //Rotation by grabbing two opposite edges and rotating 180 degrees.
            //Notated with the face that the edges are parallel to, and "a" or "b"
            //    for "major diagonal" (i.e. one of the edges is an axis)
            //    or "minor diagonal" respectively.
            EdgesXa, EdgesXb,
            EdgesYa, EdgesYb,
            EdgesZa, EdgesZb,

            //Rotation by grabbing opposite corners and rotating 120 or 240 degrees.
            //Notated with one corner, and rotation amount
            //    (clockwise while staring at the notated corner).
            //The corner is notated as "CornerXYZ", where X, Y, and Z are either
            //    "A" for the min side, or "B" for the max side.
            CornerAAA_120, CornerAAA_240,
            CornerABA_120, CornerABA_240,
            CornerBAA_120, CornerBAA_240,
            CornerBBA_120, CornerBBA_240,

            //Used for enumeration:
            Count
        };

        /*
        //Different ways to rotate a face of a cube.
        //The rotations are clockwise while facing the cube.
        enum WFC_API Rotations2D : uint8_t
        {
            R0, R90, R180, R270,

            Count
        };
        //Gets the rotation required to go from "start" to "end".
        Rotations2D WFC_API GetDelta(Rotations2D start, Rotations2D end);
        //Gets the combination of the given two rotations.
        Rotations2D WFC_API Combine(Rotations2D a, Rotations2D b);
        */

        /*//A cube face that keeps track of its local changes during 3D transformations.
        //The "local" X axis, when unrotated, goes from left to right locally
        //    and -A to +A in world space, where A is the first axis on the face
        //    (e.x. on a YZ face, the Y axis).
        //The "local" Y axis, when unrotated, goes from bottom to top locally
        //    and -B to +B in world space, where B is the second axis on the face
        //    (e.x. on an XZ face, the Z axis).
        struct WFC_API SmartFace
        {
            //Whether the local X axis (BEFORE rotation) is flipped.
            bool IsFlippedX;
            //The current "local" rotation of this face.
            Rotations2D Rot;

            //The side of the cube this face is on.
            Directions3D Side;
        };*/


        //The faces of a cube, with memory of how they have been transformed.
        struct WFC_API CubePermutation
        {
            //The faces of this cube.
            //Their positions in the array are based on the original un-transformed cube --
            //    e.x. Faces[Directions::MinX] is the face that STARTED as the MinX face.
            FacePermutation Faces[N_DIRECTIONS3D];

            //Gets the index in "Faces" for the face currently facing the given direction.
            uint_fast8_t GetFace(Directions3D dir) const;
        };


        //Reprsents a transformation that can be done to a cube without changing its shape.
        struct WFC_API Transform3D
        {
            //If true, this transform flips the cube along every axis before rotating.
            bool Invert;
            //The rotation that is applied (AFTER the inversion, if applicable).
            Rotations3D Rot;


            //Applies this transformation to the given 3D position.
            //By default centered around the origin, but you can provide a "max" value
            //    so that the transformation is in the space from 0 to "max", inclusive.
            Vector3i ApplyToPos(Vector3i pos, Vector3i maxInclusive = Vector3i::Zero()) const;

            //Gets the position of the given face after this transformation.
            Directions3D ApplyToSide(Directions3D currentSide) const;
            //Gets the new permutated face after this transformation.
            FacePermutation ApplyToFace(FacePermutation currentFace) const;
            //Gets the new permutated cube after this transformation.
            CubePermutation ApplyToCube(CubePermutation currentCube) const;
        };
    }
}