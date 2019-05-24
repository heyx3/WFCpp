#include "CubeTransform.h"

#include <array>
#include <algorithm>


using namespace WFC;
using namespace WFC::Tiled3D;


namespace
{
    //Lookup table for rotations from one face to another.
    //For example, to see what happens to the MinX face
    //    when applying RotY90, use
    //    "Lookup_DirTransforms[MinX][RotY90]".
    std::array<std::array<Directions3D, Rotations3D::Count>,
               Directions3D::Count> Lookup_DirTransforms = { {
        //TODO: Finish.
        #pragma region MinX
        {
            MinX, //None
            MinX, //AxisX_90
            MinX, //AxisX_180
            MinX, //AxisX_270
            MaxZ, //AxisY_90
            MaxX, //AxisY_180
            MinZ, //AxisY_270
            MinY, //AxisZ_90
            MaxX, //AxisZ_180
            MaxY, //AxisZ_270
            , //EdgesXa
            , //EdgesYa
            , //EdgesZa
            , //EdgesXb
            , //EdgesYb
            , //EdgesZb
            , //CornerAAA_120
            , //CornerAAA_240
            , //CornerABA_120
            , //CornerABA_240
            , //CornerBAA_120
            , //CornerBAA_240
            , //CornerBBA_120
            , //CornerBBA_240
        },
        #pragma endregion
        #pragma region MaxX
        {
            MaxX, //None
            MaxX, //AxisX_90
            MaxX, //AxisX_180
            MaxX, //AxisX_270
            MinZ, //AxisY_90
            MinX, //AxisY_180
            MaxZ, //AxisY_270
            MaxY, //AxisZ_90
            MinX, //AxisZ_180
            MinY, //AxisZ_270
            , //EdgesXa
            , //EdgesYa
            , //EdgesZa
            , //EdgesXb
            , //EdgesYb
            , //EdgesZb
            , //CornerAAA_120
            , //CornerAAA_240
            , //CornerABA_120
            , //CornerABA_240
            , //CornerBAA_120
            , //CornerBAA_240
            , //CornerBBA_120
            , //CornerBBA_240
        },
        #pragma endregion
        #pragma region MinY
        {
            MinY, //None
            MinZ, //AxisX_90
            MaxY, //AxisX_180
            MaxZ, //AxisX_270
            MinY, //AxisY_90
            MinY, //AxisY_180
            MinY, //AxisY_270
            MaxX, //AxisZ_90
            MaxY, //AxisZ_180
            MinX, //AxisZ_270
            , //EdgesXa
            , //EdgesYa
            , //EdgesZa
            , //EdgesXb
            , //EdgesYb
            , //EdgesZb
            , //CornerAAA_120
            , //CornerAAA_240
            , //CornerABA_120
            , //CornerABA_240
            , //CornerBAA_120
            , //CornerBAA_240
            , //CornerBBA_120
            , //CornerBBA_240
        },
        #pragma endregion
        #pragma region MaxY
        {
            MaxY, //None
            MaxZ, //AxisX_90
            MinY, //AxisX_180
            MinZ, //AxisX_270
            MaxY, //AxisY_90
            MaxY, //AxisY_180
            MaxY, //AxisY_270
            MinX, //AxisZ_90
            MinY, //AxisZ_180
            MaxX, //AxisZ_270
            , //EdgesXa
            , //EdgesYa
            , //EdgesZa
            , //EdgesXb
            , //EdgesYb
            , //EdgesZb
            , //CornerAAA_120
            , //CornerAAA_240
            , //CornerABA_120
            , //CornerABA_240
            , //CornerBAA_120
            , //CornerBAA_240
            , //CornerBBA_120
            , //CornerBBA_240
        },
        #pragma endregion
        #pragma region MinZ
        {
            MinZ, //None
            MaxY, //AxisX_90
            MaxZ, //AxisX_180
            MinY, //AxisX_270
            MinX, //AxisY_90
            MaxZ, //AxisY_180
            MaxX, //AxisY_270
            MinZ, //AxisZ_90
            MinZ, //AxisZ_180
            MinZ, //AxisZ_270
            , //EdgesXa
            , //EdgesYa
            , //EdgesZa
            , //EdgesXb
            , //EdgesYb
            , //EdgesZb
            , //CornerAAA_120
            , //CornerAAA_240
            , //CornerABA_120
            , //CornerABA_240
            , //CornerBAA_120
            , //CornerBAA_240
            , //CornerBBA_120
            , //CornerBBA_240
        },
        #pragma endregion
        #pragma region MaxZ
        {
            MaxZ, //None
            MinY, //AxisX_90
            MinZ, //AxisX_180
            MaxY, //AxisX_270
            MaxX, //AxisY_90
            MinZ, //AxisY_180
            MinX, //AxisY_270
            MaxZ, //AxisZ_90
            MaxZ, //AxisZ_180
            MaxZ, //AxisZ_270
            , //EdgesXa
            , //EdgesYa
            , //EdgesZa
            , //EdgesXb
            , //EdgesYb
            , //EdgesZb
            , //CornerAAA_120
            , //CornerAAA_240
            , //CornerABA_120
            , //CornerABA_240
            , //CornerBAA_120
            , //CornerBAA_240
            , //CornerBBA_120
            , //CornerBBA_240
        }
        #pragma endregion
    } };
}


Rotations2D WFC::Tiled3D::GetDelta(Rotations2D start, Rotations2D end)
{
    int_fast16_t deltaI = end - start;
    if (deltaI < 0)
        deltaI = Rotations2D::Count + deltaI;

    return (Rotations2D)deltaI;
}

Directions3D CubeTransform::GetNewDir(Directions3D face) const
{
    if (Invert)
        face = GetOpposite(face);

    return Lookup_DirTransforms[face][Rot];
}
SmartFace CubeTransform::GetNewFace(SmartFace currentFace) const
{
    //TODO: Implement.
}

uint_fast8_t SmartCube::GetFace(Directions3D dir) const
{
    for (uint_fast8_t i = 0; i < Directions3D::Count; ++i)
        if (Faces[i].Side == dir)
            return i;

    //Couldn't find that face!?
    assert(false);
    return -1;
}