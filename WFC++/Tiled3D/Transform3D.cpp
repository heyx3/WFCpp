#include "Transform3D.h"

#include <algorithm>

#include "../Vector2i.h"


using namespace WFC;
using namespace WFC::Tiled3D;

using Transformations2D = WFC::Transformations;


namespace
{
    //Lookup table for rotations from one face to another.
    //For example, to see what happens to the MinX face
    //    when applying RotY90, use
    //    "Lookup_DirTransforms[MinX][RotY90]".
    constexpr std::array<std::array<Directions3D, (size_t)N_ROTATIONS_3D>,
               N_DIRECTIONS_3D> Lookup_DirTransforms = { {
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


uint_fast8_t CubePermutation::GetFace(Directions3D dir) const
{
    for (uint_fast8_t i = 0; i < N_DIRECTIONS_3D; ++i)
        if (Faces[i].Side == dir)
            return i;

    //Couldn't find that face!?
    WFCPP_ASSERT(false);
    return -1;
}

Vector3i Transform3D::ApplyToPos(Vector3i pos, Vector3i max) const
{
    //Inversion first.
    if (Invert)
        pos = max - pos;

    //Now do rotation.
    auto iPos = max - pos;
    switch (Rot)
    {
        case Rotations3D::None:
            break;

        //Rotations around an axis are pretty easy to visualize.

        case Rotations3D::AxisX_90:
            pos = Vector3i(pos.x, iPos.z, pos.y);
            break;
        case Rotations3D::AxisX_180:
            pos = Vector3i(pos.x, iPos.y, iPos.z);
            break;
        case Rotations3D::AxisX_270:
            pos = Vector3i(pos.x, pos.z, iPos.y);
            break;

        case Rotations3D::AxisY_90:
            pos = Vector3i(pos.z, pos.y, iPos.x);
            break;
        case Rotations3D::AxisY_180:
            pos = Vector3i(iPos.x, pos.y, iPos.z);
            break;
        case Rotations3D::AxisY_270:
            pos = Vector3i(iPos.z, pos.y, pos.x);
            break;

        case Rotations3D::AxisZ_90:
            pos = Vector3i(iPos.y, pos.x, pos.z);
            break;
        case Rotations3D::AxisZ_180:
            pos = Vector3i(iPos.x, iPos.y, pos.z);
            break;
        case Rotations3D::AxisZ_270:
            pos = Vector3i(pos.y, iPos.x, pos.z);
            break;


        //Major-edge rotations flip the axis they're grabbing,
        //    and swap the other two axes.
        case Rotations3D::EdgesXa:
            pos = Vector3i(iPos.x, pos.z, pos.y);
            break;
        case Rotations3D::EdgesYa:
            pos = Vector3i(pos.z, iPos.y, pos.x);
            break;
        case Rotations3D::EdgesZa:
            pos = Vector3i(pos.y, pos.x, iPos.z);
            break;

        //Minor-edge rotations flip all axes,
        //    and swap the two axes not being grabbed.
        case Rotations3D::EdgesXb:
            pos = Vector3i(iPos.x, iPos.z, iPos.y);
            break;
        case Rotations3D::EdgesYb:
            pos = Vector3i(iPos.z, iPos.y, iPos.x);
            break;
        case Rotations3D::EdgesZb:
            pos = Vector3i(iPos.y, iPos.x, iPos.z);
            break;

        //Corner rotations swap all 3 axes between each other,
        //    sometimes flipping their values.
        case Rotations3D::CornerAAA_120:
            pos = Vector3i(pos.y, pos.z, pos.x);
            break;
        case Rotations3D::CornerAAA_240:
            pos = Vector3i(pos.z, pos.x, pos.y);
            break;
        case Rotations3D::CornerABA_120:
            pos = Vector3i(pos.z, iPos.x, iPos.y);
            break;
        case Rotations3D::CornerABA_240:
            pos = Vector3i(iPos.y, iPos.z, pos.x);
            break;
        case Rotations3D::CornerBAA_120:
            pos = Vector3i(iPos.z, iPos.x, pos.y);
            break;
        case Rotations3D::CornerBAA_240:
            pos = Vector3i(iPos.y, pos.z, iPos.x);
            break;
        case Rotations3D::CornerBBA_120:
            pos = Vector3i(pos.y, iPos.z, iPos.x);
            break;
        case Rotations3D::CornerBBA_240:
            pos = Vector3i(iPos.z, pos.x, iPos.y);
            break;

        default: WFCPP_ASSERT(false); break;
    }

    return pos;
}

Directions3D Transform3D::ApplyToSide(Directions3D side) const
{
    if (Invert)
        side = GetOpposite(side);
    side = Lookup_DirTransforms[side][(int)Rot];
    return side;
}
FacePermutation Transform3D::ApplyToFace(FacePermutation face) const
{
    using smallU_t = uint_fast8_t;
    using smallI_t = int_fast8_t;

    auto oldSide = face.Side;
    smallU_t axisMainOld, axisFace1Old, axisFace2Old;
    GetAxes(oldSide, axisMainOld, axisFace1Old, axisFace2Old);

    auto newSide = ApplyToSide(oldSide);
    smallU_t axisMainNew, axisFace1New, axisFace2New;
    GetAxes(newSide, axisMainNew, axisFace1New, axisFace2New);

    //Get the world-space points on the input face.
    //Their components are 0 if on the min of that axis, and 1 if on the max.
    std::array<Vector3i, 4> oldCornerPoses;

    //Build the points algorithmically to make my life simpler.
    {
        //This face's axis has the same value for all four points.
        smallU_t myAxisValue = IsMin(oldSide) ? 0 : 1;
        for (int i = 0; i < N_FACE_POINTS; ++i)
            oldCornerPoses[i][axisMainOld] = myAxisValue;

        //Fill in the "first" axis values.
        oldCornerPoses[FacePoints::AA][axisFace1Old] = 0;
        oldCornerPoses[FacePoints::AB][axisFace1Old] = 0;
        oldCornerPoses[FacePoints::BA][axisFace1Old] = 1;
        oldCornerPoses[FacePoints::BB][axisFace1Old] = 1;

        //Fill in the "second" axis values.
        oldCornerPoses[FacePoints::AA][axisFace2Old] = 0;
        oldCornerPoses[FacePoints::AB][axisFace2Old] = 1;
        oldCornerPoses[FacePoints::BA][axisFace2Old] = 0;
        oldCornerPoses[FacePoints::BB][axisFace2Old] = 1;
    }

    //Apply this transform to the face points.
    auto newCornerPoses = oldCornerPoses;
    for (auto& p : newCornerPoses)
        p = ApplyToPos(p, Vector3i(1, 1, 1));

    //For each point, figure out where it is now on the new face.
    //Swap the point ID's accordingly.
    FaceCorners oldPointIDs, newPointIDs;
    oldPointIDs = face.Points;
    #if defined(WFCPP_DEBUG)
        //Initialize the data to -1 so that
        //    it stands out if one stays uninitialized.
        memset(newPointIDs.data(), 0xff, sizeof(PointID) * newPointIDs.size());
    #endif
    for (smallU_t pI = 0; pI < N_FACE_POINTS; ++pI)
    {
        Vector3i worldPos = newCornerPoses[pI];
        bool isAxis1Min = worldPos[axisFace1New] == 0,
             isAxis2Min = worldPos[axisFace2New] == 0;

        auto place = MakeFacePoint(isAxis1Min, isAxis2Min);
        newPointIDs[place] = oldPointIDs[pI];
    }
    #if defined(WFCPP_DEBUG)
        //Double-check that every old point mapped to a unique new point.
        for (PointID newID : newPointIDs)
            WFCPP_ASSERT(newID != ~PointID{ 0 });
    #endif

    //Output the mapped face data.
    face.Points = newPointIDs;
    face.Side = newSide;
    return face;
}
CubePermutation Transform3D::ApplyToCube(CubePermutation cube) const
{
    for (auto& face : cube.Faces)
        face = ApplyToFace(face);
    return cube;
}