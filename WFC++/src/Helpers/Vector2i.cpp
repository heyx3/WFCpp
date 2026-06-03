#include "../../include/Helpers/Vector2i.h"

#include <algorithm>
#include <optional>
#include <unordered_map>

using namespace WFC;


Transformations WFC::Invert(Transformations t)
{
	switch (t)
	{
		case Transformations::Rotate90CW: return Transformations::Rotate270CW;
		case Transformations::Rotate270CW: return Transformations::Rotate90CW;

		case Transformations::Rotate180: return Transformations::Rotate180;

		case Transformations::FlipX: return Transformations::FlipX;
		case Transformations::FlipY: return Transformations::FlipY;

        case Transformations::FlipDiag1: return Transformations::FlipDiag1;
        case Transformations::FlipDiag2: return Transformations::FlipDiag2;

		case Transformations::None: return Transformations::None;

		default: WFCPP_ASSERT(false); return Transformations::None;
	}
}
bool WFC::WillSwapAxes(Transformations t)
{
    switch (t)
    {
        case Transformations::None:
        case Transformations::Rotate180:
        case Transformations::FlipX:
        case Transformations::FlipY:
            return false;

        case Transformations::Rotate90CW:
        case Transformations::Rotate270CW:
        case Transformations::FlipDiag1:
        case Transformations::FlipDiag2:
            return true;

        default: WFCPP_ASSERT(false); return false;
    }
}
const char* WFC::ToString(Transformations t)
{
    switch (t)
    {
        case Transformations::Rotate90CW: return "Rot90CW";
        case Transformations::Rotate180: return "Rot180";
        case Transformations::Rotate270CW: return "Rot270CW";
        case Transformations::FlipX: return "FlipX";
        case Transformations::FlipY: return "FlipY";
        case Transformations::FlipDiag1: return "FlipDiag1";
        case Transformations::FlipDiag2: return "FlipDiag2";
        case Transformations::None: return "None";

        default: WFCPP_ASSERT(false); return "UNKNOWN";
    }
}
bool WFC::IsReflection(Transformations t)
{
    switch (t)
    {
        case Transformations::None:
        case Transformations::Rotate90CW:
        case Transformations::Rotate180:
        case Transformations::Rotate270CW:
            return false;

        case Transformations::FlipX:
        case Transformations::FlipY:
        case Transformations::FlipDiag1:
        case Transformations::FlipDiag2:
            return true;

        default: WFCPP_ASSERT(false); return false;
    }
}


//Data object that precomputes all possible transform sequences.
struct TransformApplicationCache2D
{
    std::array<Transformations, Transformations::Count * Transformations::Count> cachedAnswers;
    Transformations& GetAnswer(const Transformations& a, const Transformations& b)
    {
        return cachedAnswers[static_cast<int>(a) + (static_cast<int>(b) * Transformations::Count)];
    }

    TransformApplicationCache2D()
    {
        //There are 64 possible transformations.
        //For each pair of transforms, apply them to the four corner points
        //    and deduce the resulting transform by their final arrangement.

        //Corner points are binary vectors: along each axis, 0 for min side and 1 for max side.
        //Therefore a corner position can be represented with a 2-bit integer, from 0 to 3.
        //Four corners require 8 bits (and could be packed even smaller but it's not worth the effort).
        using CornerPoints = std::array<Vector2i, 4>;
        using CornersID = uint_fast8_t;
        auto cornerPointsToID = [&](const CornerPoints& c) -> CornersID
        {
            CornersID output = 0;
            for (CornersID i = 0; i < 4; ++i)
            {
                CornersID pointBits = 0;
                for (CornersID axis = 0; axis < 2; ++axis)
                    pointBits |= (c[i][axis]) << axis;

                CornersID firstBit = (i * 2);
                output |= pointBits << firstBit;
            }
            return output;
        };
        auto cornerPointsInitial = [&]() -> CornerPoints
        {
            return {
                //The specific ordering isn't important; just has to be the same every time.
                Vector2i{ 0, 0 },
                Vector2i{ 0, 1 },
                Vector2i{ 1, 0 },
                Vector2i{ 1, 1 }
            };
        };
        auto cornerPointsTransform = [&](const CornerPoints& src, Transformations tr) -> CornerPoints
        {
            CornerPoints dest;
            for (int i = 0; i < 4; ++i)
                dest[i] = src[i].Transform(tr, Vector2i{ 2, 2 });
            return dest;
        };

        //Define a mapping from every possible corner arrangement to the transform that created it.
        std::unordered_map<CornersID, Transformations> transformByCornerArrangement;
        for (int trI = 0; trI < Transformations::Count; ++trI)
        {
            Transformations tr = static_cast<Transformations>(trI);

            auto transformedCorners = cornerPointsTransform(cornerPointsInitial(), tr);
            auto cornersID = cornerPointsToID(transformedCorners);

            WFCPP_ASSERT(transformByCornerArrangement.find(cornersID) == transformByCornerArrangement.end());
            transformByCornerArrangement[cornersID] = tr;
        }

        //Finally, look up the meaning of every possible pair of transforms.
        for (int trI1 = 0; trI1 < Transformations::Count; ++trI1)
        {
            Transformations tr1 = static_cast<Transformations>(trI1);
            auto cornersTransformed1 = cornerPointsTransform(cornerPointsInitial(), tr1);

            for (int trI2 = 0; trI2 < Transformations::Count; ++trI2)
            {
                Transformations tr2 = static_cast<Transformations>(trI2);
                auto cornersTransformedFinal = cornerPointsTransform(cornersTransformed1, tr2);
                
                GetAnswer(tr1, tr2) = transformByCornerArrangement.at(cornerPointsToID(cornersTransformedFinal));
            }
        }
    }
};
Transformations WFC::CombineTr2D(Transformations a, Transformations b)
{
    static TransformApplicationCache2D cache;
    return cache.GetAnswer(a, b);
}

Vector2i Vector2i::Transform(Transformations trnsf, Vector2i size) const
{
	switch (trnsf)
	{
		case Transformations::Rotate90CW: {
			//Swap X and Y, then mirror the new X.
			Vector2i result(y, x);
			result.x = size.y - result.x - 1;
			return result;
        }

		case Transformations::Rotate270CW: {
			//Swap X and Y, then mirror the new Y.
			Vector2i result(y, x);
			result.y = size.x - result.y - 1;
			return result;
        }

		case Transformations::Rotate180:
			//Just mirror along both axes.
			return size - *this - 1;

		case Transformations::FlipX:
			return Vector2i(size.x - x - 1, y);
		case Transformations::FlipY:
			return Vector2i(x, size.y - y - 1);

        case Transformations::FlipDiag1:
            return Vector2i(y, x);
        case Transformations::FlipDiag2:
            return Vector2i(size.y - y - 1, size.x - x - 1);

		case Transformations::None:
			return *this;

		default: WFCPP_ASSERT(false); return *this;
	}
}