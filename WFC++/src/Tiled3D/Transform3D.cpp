#include "../../include/Tiled3D/Transform3D.h"

#include <algorithm>
#include <unordered_map>

#include "../../include/Helpers/Vector2i.h"


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

Rotations3D CombineRotations(Rotations3D a, Rotations3D b)
{
    //We already have an efficient solution for transforms,
    //     which are a superset of rotations.
    return Transform3D{ false, a }.Then(Transform3D{ false, b }).Rot;
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
    std::array<Vector3i, 4> oldCornerPoses, oldEdgePoses;

    //Build the points algorithmically to make my life simpler.
    {
        //This face's axis has the same value for all points.
        smallU_t myAxisValue = IsMin(oldSide) ? 0 : 1;
        for (int i = 0; i < N_FACE_POINTS; ++i)
        {
            oldCornerPoses[i][axisMainOld] = myAxisValue;
            oldEdgePoses[i][axisMainOld] = myAxisValue;
        }

        //For the corners:
        //   Fill in the "first" face axis values
        oldCornerPoses[FacePoints::AA][axisFace1Old] = 0;
        oldCornerPoses[FacePoints::AB][axisFace1Old] = 0;
        oldCornerPoses[FacePoints::BA][axisFace1Old] = 1;
        oldCornerPoses[FacePoints::BB][axisFace1Old] = 1;
        //   Fill in the "second" face axis values
        oldCornerPoses[FacePoints::AA][axisFace2Old] = 0;
        oldCornerPoses[FacePoints::AB][axisFace2Old] = 1;
        oldCornerPoses[FacePoints::BA][axisFace2Old] = 0;
        oldCornerPoses[FacePoints::BB][axisFace2Old] = 1;
        //For the edges (using 2 for the whole face and 1 for halfway along the edge):
        //  Fill in the "first" face axis values
        oldEdgePoses[FacePoints::AA][axisFace1Old] = 1;
        oldEdgePoses[FacePoints::AB][axisFace1Old] = 1;
        oldEdgePoses[FacePoints::BA][axisFace1Old] = 0;
        oldEdgePoses[FacePoints::BB][axisFace1Old] = 2;
        //  Fill in the "second" face axis values
        oldEdgePoses[FacePoints::AA][axisFace2Old] = 0;
        oldEdgePoses[FacePoints::AB][axisFace2Old] = 2;
        oldEdgePoses[FacePoints::BA][axisFace2Old] = 1;
        oldEdgePoses[FacePoints::BB][axisFace2Old] = 1;
    }

    //Apply this transform to the face points.
    auto newCornerPoses = oldCornerPoses,
         newEdgePoses = oldEdgePoses;
    for (auto& p : newCornerPoses)
        p = ApplyToPos(p, Vector3i(1, 1, 1));
    for (auto& p : newEdgePoses)
        p = ApplyToPos(p, Vector3i(2, 2, 2));

    //For each point, figure out where it is now on the new face.
    //Swap the point ID's accordingly.
    FaceIdentifiers oldIDs, newIDs;
    oldIDs = face.Points;
    #if defined(WFCPP_DEBUG)
        //Initialize the data to -1 so that
        //    it stands out if any is uninitialized.
        memset(newIDs.Corners.data(), 0xff, sizeof(PointID) * newIDs.Corners.size());
        memset(newIDs.Edges.data(), 0xff, sizeof(PointID) * newIDs.Edges.size());
    #endif
    for (smallU_t cornerI = 0; cornerI < N_FACE_POINTS; ++cornerI)
    {
        Vector3i worldPos = newCornerPoses[cornerI];
        bool isAxis1Min = worldPos[axisFace1New] == 0,
             isAxis2Min = worldPos[axisFace2New] == 0;

        auto place = MakeCornerFacePoint(isAxis1Min, isAxis2Min);
        auto& outCorner = newIDs.Corners[place];
        WFCPP_ASSERT(outCorner == ~PointID{ 0 });
        outCorner = oldIDs.Corners[cornerI];
    }
    for (smallU_t edgeI = 0; edgeI < N_FACE_POINTS; ++edgeI)
    {
        Vector3i worldPos = newEdgePoses[edgeI];
        bool isParallelToAxis1 = worldPos[axisFace1New] == 1,
             isMinEdge = worldPos[isParallelToAxis1 ? axisFace2New : axisFace1New] == 0;

        auto place = MakeEdgeFacePoint(isParallelToAxis1, isMinEdge);
        auto& outEdge = newIDs.Edges[place];
        WFCPP_ASSERT(outEdge == ~PointID{ 0 });
        outEdge = oldIDs.Edges[edgeI];
    }
    #if defined(WFCPP_DEBUG)
        //Double-check that every old point mapped to a unique new point.
        for (PointID newID : newIDs.Corners)
            WFCPP_ASSERT(newID != ~PointID{ 0 });
        for (PointID newID : newIDs.Edges)
            WFCPP_ASSERT(newID != ~PointID{ 0 });
    #endif

    //Output the mapped face data.
    face.Points = newIDs;
    face.Side = newSide;
    return face;
}
CubePermutation Transform3D::ApplyToCube(CubePermutation cube) const
{
    for (auto& face : cube.Faces)
        face = ApplyToFace(face);
    return cube;
}

//Data object that precomputes all possible transform sequences.
struct TransformApplicationCache
{
    std::array<Transform3D, N_TRANSFORMS * N_TRANSFORMS> cachedAnswers;
    Transform3D& GetAnswer(const Transform3D& a, const Transform3D& b)
    {
        auto aID = TransformSet::ToBitIdx(a),
             bID = TransformSet::ToBitIdx(b);
        return cachedAnswers[aID + (bID * N_TRANSFORMS)];
    }

    TransformApplicationCache()
    {
        //Enumerating this is a nightmare -- 2304 cases!
        //Instead, for each pair of transforms, apply them to the eight corner points,
        //    and deduce the resulting transform by their final arrangement.

        //Corner points are binary vectors: along each axis, 0 for min side and 1 for max side.
        //Therefore a corner position can be represented with a 3-bit integer, from 0 to 7.
        //Eight corners require 24 bits (and could be packed even smaller but it's not worth the effort).
        using CornerPoints = std::array<Vector3i, 8>;
        using CornersID = uint_fast32_t;
        auto cornerPointsToID = [&](const CornerPoints& c) -> CornersID
        {
            CornersID output = 0;
            for (CornersID i = 0; i < 8; ++i)
            {
                CornersID pointBits = 0;
                for (CornersID axis = 0; axis < 3; ++axis)
                    pointBits |= (c[i][axis]) << axis;

                CornersID firstBit = (i * 3);
                output |= pointBits << firstBit;
            }
            return output;
        };
        auto cornerPointsInitial = [&]() -> CornerPoints
        {
            CornerPoints output;
            for (int cornerI = 0; cornerI < 8; ++cornerI)
            {
                //The specific ordering isn't important; just has to be the same every time.
                output[cornerI] = {
                    (cornerI % 2),     // 01010101
                    (cornerI / 4),     // 00001111
                    (cornerI / 2) % 2  // 00110011
                };
            }
            return output;
        };
        auto cornerPointsTransform = [&](const CornerPoints& src, const Transform3D& tr) -> CornerPoints
        {
            CornerPoints dest;
            for (int i = 0; i < 8; ++i)
                dest[i] = tr.ApplyToPos(src[i], Vector3i{ 1, 1, 1 });
            return dest;
        };

        //Define a mapping from corner arrangement to the transform that created it.
        std::unordered_map<CornersID, Transform3D> transformByCornerArrangement;
        for (int shouldInvert = 0; shouldInvert < 2; ++shouldInvert)
        {
            for (int rotID = 0; rotID < N_ROTATIONS_3D; ++rotID)
            {
                Transform3D tr{ shouldInvert == 1, static_cast<Rotations3D>(rotID) };
                
                auto transformedCorners = cornerPointsTransform(cornerPointsInitial(), tr);
                auto cornersID = cornerPointsToID(transformedCorners);

                assert(transformByCornerArrangement.find(cornersID) == transformByCornerArrangement.end());
                transformByCornerArrangement[cornersID] = tr;
            }
        }

        //Now check every possible combination of transforms.
        for (int shouldInvert1 = 0; shouldInvert1 < 2; ++shouldInvert1)
        {
            for (int rotID1 = 0; rotID1 < N_ROTATIONS_3D; ++rotID1)
            {
                Transform3D tr1{ shouldInvert1 == 1, static_cast<Rotations3D>(rotID1) };
                auto cornersTransformed1 = cornerPointsTransform(cornerPointsInitial(), tr1);

                for (int shouldInvert2 = 0; shouldInvert2 < 2; ++shouldInvert2)
                {
                    for (int rotID2 = 0; rotID2 < N_ROTATIONS_3D; ++rotID2)
                    {
                        Transform3D tr2{ shouldInvert2 == 1, static_cast<Rotations3D>(rotID2) };
                        auto cornersTransformedFinal = cornerPointsTransform(cornersTransformed1, tr2);

                        Transform3D effectiveTr = transformByCornerArrangement.at(cornerPointsToID(cornersTransformedFinal));
                        GetAnswer(tr1, tr2) = effectiveTr;
                    }
                }
            }
        }
    }
};
Transform3D Transform3D::Then(const Transform3D& next) const
{
    static TransformApplicationCache cache;
    return cache.GetAnswer(*this, next);
}

FacePoints WFC::Tiled3D::TransformFaceCorner(FacePoints p, Directions3D dir, Transformations tr2D)
{
    //Faces can be left-handed or right-handed.
    //Flip our situation so we're always left-handed like the larger coordinate system.
	switch (dir)
	{
		case Directions3D::MaxX:
		case Directions3D::MinY:
		case Directions3D::MaxZ:
			return TransformFaceCorner(p, GetOpposite(dir), WFC::Invert(tr2D));
		
		case Directions3D::MinX:
		case Directions3D::MaxY:
		case Directions3D::MinZ:
			break;
		
		default: assert(false);
	}

	#define WFCPP_RETURN(aa, ab, ba, bb) switch (p) { \
		case FacePoints::AA: return FacePoints::aa; \
		case FacePoints::AB: return FacePoints::ab; \
		case FacePoints::BA: return FacePoints::ba; \
		case FacePoints::BB: return FacePoints::bb; \
		default: assert(false); return aa; \
	}

    switch (tr2D)
    {
		case Transformations::None: WFCPP_RETURN(AA, AB, BA, BB);
		case Transformations::Rotate90CW: WFCPP_RETURN(AB, BB, AA, BA);
		case Transformations::Rotate180: WFCPP_RETURN(BB, BA, AB, AA);
		case Transformations::Rotate270CW: WFCPP_RETURN(BA, AA, BB, AB);
		case Transformations::FlipX: WFCPP_RETURN(BA, BB, AA, AB);
		case Transformations::FlipY: WFCPP_RETURN(AB, AA, BB, BA);
		case Transformations::FlipDiag1: WFCPP_RETURN(AA, BA, AB, BB);
		case Transformations::FlipDiag2: WFCPP_RETURN(BB, AB, BA, AA);
		default: assert(false); return p;
    }
}
FacePoints WFC::Tiled3D::TransformFaceEdge(FacePoints p, Directions3D dir, Transformations tr2D)
{
    //Faces can be left-handed or right-handed.
    //Flip our situation so we're always left-handed like the larger coordinate system.
	switch (dir)
	{
		case Directions3D::MaxX:
		case Directions3D::MinY:
		case Directions3D::MaxZ:
			return TransformFaceEdge(p, GetOpposite(dir), WFC::Invert(tr2D));
		
		case Directions3D::MinX:
		case Directions3D::MaxY:
		case Directions3D::MinZ:
			break;
		
		default: assert(false);
	}

	#define WFCPP_RETURN(aa, ab, ba, bb) switch (p) { \
		case FacePoints::AA: return FacePoints::aa; \
		case FacePoints::AB: return FacePoints::ab; \
		case FacePoints::BA: return FacePoints::ba; \
		case FacePoints::BB: return FacePoints::bb; \
		default: assert(false); return aa; \
	}

    switch (tr2D)
    {
		case Transformations::None: WFCPP_RETURN(AA, AB, BA, BB);
		case Transformations::Rotate90CW: WFCPP_RETURN(BA, BB, AB, AA);
		case Transformations::Rotate180: WFCPP_RETURN(AB, AA, BB, BA);
		case Transformations::Rotate270CW: WFCPP_RETURN(BB, BA, AA, AB);
		case Transformations::FlipX: WFCPP_RETURN(AA, AB, BB, BA);
		case Transformations::FlipY: WFCPP_RETURN(AB, AA, BA, BB);
		case Transformations::FlipDiag1: WFCPP_RETURN(BA, BB, AA, AB);
		case Transformations::FlipDiag2: WFCPP_RETURN(BB, BA, AB, AA);
		default: assert(false); return p;
    }
}