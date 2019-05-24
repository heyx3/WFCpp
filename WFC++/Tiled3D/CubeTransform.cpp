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
            MaxX, //EdgesXa
            MaxX, //EdgesXb
            MinZ, //EdgesYa
            MaxZ, //EdgesYb
            MinY, //EdgesZa
            MaxY, //EdgesZb
            MinZ, //CornerAAA_120
            MinY, //CornerAAA_240
            MaxY, //CornerABA_120
            MinZ, //CornerABA_240
            MaxY, //CornerBAA_120
            MaxZ, //CornerBAA_240
            MaxZ, //CornerBBA_120
            MinY, //CornerBBA_240
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
            MinX, //EdgesXa
            MinX, //EdgesXb
            MaxZ, //EdgesYa
            MinZ, //EdgesYb
            MaxY, //EdgesZa
            MinY, //EdgesZb
            MaxZ, //CornerAAA_120
            MaxY, //CornerAAA_240
            MinY, //CornerABA_120
            MaxZ, //CornerABA_240
            MinY, //CornerBAA_120
            MinZ, //CornerBAA_240
            MinZ, //CornerBBA_120
            MaxY, //CornerBBA_240
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
            MinZ, //EdgesXa
            MaxZ, //EdgesXb
            MaxY, //EdgesYa
            MaxY, //EdgesYb
            MinX, //EdgesZa
            MaxX, //EdgesZb
            MinX, //CornerAAA_120
            MinZ, //CornerAAA_240
            MaxZ, //CornerABA_120
            MaxX, //CornerABA_240
            MinZ, //CornerBAA_120
            MaxX, //CornerBAA_240
            MinX, //CornerBBA_120
            MaxZ, //CornerBBA_240
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
            MaxZ, //EdgesXa
            MinZ, //EdgesXb
            MinY, //EdgesYa
            MinY, //EdgesYb
            MaxX, //EdgesZa
            MinX, //EdgesZb
            MaxX, //CornerAAA_120
            MaxZ, //CornerAAA_240
            MinZ, //CornerABA_120
            MinX, //CornerABA_240
            MaxZ, //CornerBAA_120
            MinX, //CornerBAA_240
            MaxX, //CornerBBA_120
            MinZ, //CornerBBA_240
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
            MinY, //EdgesXa
            MaxY, //EdgesXb
            MinX, //EdgesYa
            MaxX, //EdgesYb
            MaxZ, //EdgesZa
            MaxZ, //EdgesZb
            MinY, //CornerAAA_120
            MinX, //CornerAAA_240
            MinX, //CornerABA_120
            MaxY, //CornerABA_240
            MaxX, //CornerBAA_120
            MinY, //CornerBAA_240
            MaxY, //CornerBBA_120
            MaxX, //CornerBBA_240
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
            MaxY, //EdgesXa
            MinY, //EdgesXb
            MaxX, //EdgesYa
            MinX, //EdgesYb
            MinZ, //EdgesZa
            MinZ, //EdgesZb
            MaxY, //CornerAAA_120
            MaxX, //CornerAAA_240
            MaxX, //CornerABA_120
            MinY, //CornerABA_240
            MinX, //CornerBAA_120
            MaxY, //CornerBAA_240
            MinY, //CornerBBA_120
            MinX, //CornerBBA_240
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