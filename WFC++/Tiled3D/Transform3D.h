#pragma once

#include "../Platform.h"
#include "../Vector3i.h"
#include <stdint.h>

namespace WFC
{
    namespace Tiled3D
    {
        //Unique identifiers for points on a tile face.
        //Each type of face should have its own unique PointIDs --
        //    anywhere from 1 to 4, depending on the face's symmetry.
        //Four of these, can uniquely identify a face permutation.
        using PointID = uint_fast32_t;


        //Note that we're assuming a left-handed coordinate system.
        //Conceptually, we visualize X as rightward, Z as forward, and Y as upwards.
        //This matches the coordinate system used by the Unity3D game engine.

        //Also note that most/all the enums below have a specific ordering
        //    that a number of functions and lookup tables rely on.
        //Don't change them!


        //The four points on a face, ordered based on world-space
        //    to remove ambiguity.
        //This ordering makes it easier to see if two opposite faces
        //    will line up against each other.
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
            MinX, MaxX,
            MinY, MaxY,
            MinZ, MaxZ,

            #define N_DIRECTIONS3D 6
        };
        inline bool WFC_API IsMin(Directions3D dir) { return (uint_fast8_t)dir % 2 == 0; }
        inline bool WFC_API IsMax(Directions3D dir) { return (uint_fast8_t)dir % 2 == 1; }
        inline uint_fast8_t GetAxisIndex(Directions3D dir) { return (uint_fast8_t)dir / 2; }
        inline Directions3D WFC_API GetOpposite(Directions3D dir) { return (Directions3D)(IsMin(dir) ? (dir + 1) : (dir - 1)); }
        inline Vector3i GetFaceDirection(Directions3D dir)
        {
            switch (dir)
            {
                default: assert(false);

                case Directions3D::MinX: return Vector3i(-1, 0, 0);
                case Directions3D::MaxX: return Vector3i(1, 0, 0);

                case Directions3D::MinY: return Vector3i(0, -1, 0);
                case Directions3D::MaxY: return Vector3i(0, 1, 0);

                case Directions3D::MinZ: return Vector3i(0, 0, -1);
                case Directions3D::MaxZ: return Vector3i(0, 0, 1);
            }
        }



        //A specific permutation of a face.
        struct WFC_API FacePermutation
        {
            //The side of the cube this face is on.
            Directions3D Side;

            //The four points of this face. Indexed with the "FacePoints" enum.
            //Two faces can line up against each other if their corresponding points match.
            PointID Points[4];


            #pragma region Hashing/equality-testing
            //Gets the hash value for an instance.
            //Allows this type to be used as a Dictionary<> key.
            inline unsigned int operator()(const FacePermutation& f) const
            {
                //Use the Vector3i hasher since it already exists.
                Vector3i v1((int)Side, (int)Points[0], (int)Points[1]),
                         v2((int)Points[2], (int)Points[3], (int)Vector3i()(v1));
                return Vector3i()(v2);
            }

            inline bool operator==(const FacePermutation& f2) const
            {
                return (Side == f2.Side) &
                       (memcmp(Points, f2.Points, sizeof(PointID) * 4) == 0);
            }
            inline bool operator!=(const FacePermutation& f2) const { return !(operator==(f2)); }
            #pragma endregion
        };


        //Different ways to rotate a cube while keeping it axis-aligned.
        enum class WFC_API Rotations3D : uint8_t
        {
            //Note that some functions and lookup tables depend on the exact ordering here!

            //0- or 360-degree rotation.
            None,

            //Rotation along an axis, clockwise when looking at the positive face.
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


        //Reprsents a transformation that can be done to a cube (while keeping it axis-aligned).
        struct WFC_API Transform3D
        {
            //If true, this transform flips the cube along every axis before rotating.
            bool Invert = false;
            //The rotation that is applied (AFTER the inversion, if applicable).
            Rotations3D Rot = Rotations3D::None;


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

        inline bool operator==(Transform3D t1, Transform3D t2)
        {
            return (t1.Invert == t2.Invert) & (t1.Rot == t2.Rot);
        }
        inline bool operator!=(Transform3D t1, Transform3D t2)
        {
            return !operator==(t1, t2);
        }
    }
}