#include "WFC_DataReflection.h"

namespace W3D = WFC::Tiled3D;

W3D::CubePermutation FWFC_Cube::ToInternal() const
{
	W3D::CubePermutation output;

	for (int faceI = 0; faceI < N_DIRECTIONS3D; ++faceI)
	{
		output.Faces[faceI].Side = static_cast<W3D::Directions3D>(faceI);

		auto points =
			(faceI == W3D::MinX) ? MinX :
		   ((faceI == W3D::MaxX) ? MaxX :
		   ((faceI == W3D::MinY) ? MinY :
		   ((faceI == W3D::MaxY) ? MaxY :
		   ((faceI == W3D::MinZ) ? MinZ :
		    MaxZ
		))));
		output.Faces[faceI].Points[W3D::AA] = points.AA;
		output.Faces[faceI].Points[W3D::AB] = points.AB;
		output.Faces[faceI].Points[W3D::BA] = points.BA;
		output.Faces[faceI].Points[W3D::BB] = points.BB;
	}

	return output;
}
