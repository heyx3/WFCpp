#include "Vector2i.h"

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

		case Transformations::None: return Transformations::None;

		default: assert(false); return Transformations::None;
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
        case Transformations::None: return "None";

        default: assert(false); return "UNKNOWN";
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

		case Transformations::None:
			return *this;

		default: assert(false); return *this;
	}
}