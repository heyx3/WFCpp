#include "WfcTile.h"

#include "Vector2i.h"


WFC::Tiled3D::FacePoints FWfcTileFace::GetPrototypeCorner(WFC::Tiled3D::FacePoints thisCorner) const
{
    //Create a vector representing the corner, and transform it by the orientation.
    //Then, convert that transformed corner back into an enum value.
    auto dir = WFC::Tiled3D::MakeFaceVector(thisCorner);
    dir = dir.Transform(static_cast<WFC::Transformations>(PrototypeOrientation));
    return WFC::Tiled3D::MakeFacePoint(dir.x < 0, dir.y < 0);
}