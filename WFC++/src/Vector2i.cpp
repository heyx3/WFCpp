#include "Helpers/Vector2i.h"

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