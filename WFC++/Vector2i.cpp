#include "Vector2i.h"

using namespace WFC;


Transformations Invert(Transformations t)
{
	switch (t)
	{
		case Transformations::Rotate90CW: return Transformations::Rotate270CW;
		case Transformations::Rotate270CW: return Transformations::Rotate90CW;

		case Transformations::Rotate180: return Transformations::Rotate180;

		case Transformations::MirrorX: return Transformations::MirrorX;
		case Transformations::MirrorY: return Transformations::MirrorY;

		case Transformations::None: return Transformations::None;

		default: assert(false); return Transformations::None;
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

		case Transformations::MirrorX:
			return Vector2i(size.x - x - 1, y);
		case Transformations::MirrorY:
			return Vector2i(x, size.y - y - 1);

		case Transformations::None:
			return *this;

		default: assert(false); return *this;
	}
}