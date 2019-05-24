#pragma once

#include "../Platform.h"
#include "../Vector3i.h"

namespace WFC
{
    namespace Tiled3D
    {
        //Unique identifiers for each tile cube.
        using TileID = uint_fast32_t;
        //Unique identifiers for tile faces. Two faces can be 
        using FaceID = uint_fast32_t;


        //Note that we're assuming a left-handed coordinate system where "up" is +Y.
        //This matches the coordinate system used by the Unity3D game engine.

        //Different ways to rotate a cube that preserve its shape.
        enum WFC_API Rotations3D : uint8_t
        {
            //Note that there are lookup tables in the cpp file
            //    that depend on the exact ordering of these items.
            //So don't re-order them without reordering those tables!


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
            EdgesXa, EdgesYa, EdgesZa,
            EdgesXb, EdgesYb, EdgesZb,

            //Rotation by grabbing opposite corners and rotating 120 or 240 degrees.
            //Notated with one corner and rotation amount.
            //The corner is notated as "CornerXYZ", where X, Y, and Z are either
            //    "A" for the min side, or "B" for the max side.
            CornerAAA_120, CornerAAA_240,
            CornerABA_120, CornerABA_240,
            CornerBAA_120, CornerBAA_240,
            CornerBBA_120, CornerBBA_240,

            //Used for enumeration:
            Count
        };

        //Different ways to rotate a face of a cube.
        //The rotations are clockwise while facing the cube.
        enum WFC_API Rotations2D : uint8_t
        {
            R0, R90, R180, R270,

            Count
        };

        //Gets the rotation required to go from "start" to "end".
        Rotations2D WFC_API GetDelta(Rotations2D start, Rotations2D end);

        //The different axis directions/faces of a cube.
        enum WFC_API Directions3D : uint8_t
        {
            //Note that some functions and lookup tables depend on the exact numbering here!

            MinX, MaxX,
            MinY, MaxY,
            MinZ, MaxZ,

            //Used for enumeration:
            Count
        };


        inline bool WFC_API IsMin(Directions3D dir) { return (uint_fast8_t)dir % 2 == 0; }
        inline bool WFC_API IsMax(Directions3D dir) { return (uint_fast8_t)dir % 2 == 1; }
        inline Directions3D WFC_API GetOpposite(Directions3D dir) { return (IsMin(dir) ? (dir + 1) : (dir - 1)); }


        //A cube face that keeps track of its local changes during 3D transformations.
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
        };

        //Reprsents a transformation that can be done to a cube without changing its shape.
        struct WFC_API CubeTransform
        {
            //If true, this transform flips the cube along every axis.
            bool Invert;
            //The rotation that is applied, AFTER the inversion if applicable.
            Rotations3D Rot;


            //Gets the position of the given face after this transformation.
            Directions3D GetNewDir(Directions3D face) const;
            //Applies this transformation to the given face.
            SmartFace GetNewFace(SmartFace currentFace) const;
        };


        //The faces of a cube, with memory of how they have been transformed.
        struct WFC_API SmartCube
        {
            //The faces of this cube.
            //Their positions in the array are based on the original un-transformed cube --
            //    e.x. Faces[Directions::MinX] is the face that STARTED as the MinX face.
            SmartFace Faces[Directions3D::Count];

            //Gets the index of the face currently facing the given direction.
            uint_fast8_t GetFace(Directions3D dir) const;
        };
    }
}