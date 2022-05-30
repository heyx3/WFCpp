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

void FWfcTile::GetSupportedTransforms(TSet<FWFC_Transform3D>& output) const
{
    buffer_usedRotations.Empty();
    
    //Add categories of rotations.
    output.Add(FWFC_Transform3D(WFC_Rotations3D::None));
    if (UseAllRotations | UseXAxisRotations) buffer_usedRotations.Append({
        WFC_Rotations3D::AxisX_90,
        WFC_Rotations3D::AxisX_180,
        WFC_Rotations3D::AxisX_270
    });
    if (UseAllRotations | UseYAxisRotations) buffer_usedRotations.Append({
        WFC_Rotations3D::AxisY_90,
        WFC_Rotations3D::AxisY_180,
        WFC_Rotations3D::AxisY_270
    });
    if (UseAllRotations | UseZAxisRotations) buffer_usedRotations.Append({
        WFC_Rotations3D::AxisZ_90,
        WFC_Rotations3D::AxisZ_180,
        WFC_Rotations3D::AxisZ_270
    });
    if (UseAllRotations | UseEdgeRotations) buffer_usedRotations.Append({
        WFC_Rotations3D::EdgesXa,
        WFC_Rotations3D::EdgesXb,
        WFC_Rotations3D::EdgesYa,
        WFC_Rotations3D::EdgesYb,
        WFC_Rotations3D::EdgesZa,
        WFC_Rotations3D::EdgesZb
    });
    if (UseAllRotations | UseCornerRotations) buffer_usedRotations.Append({
        WFC_Rotations3D::CornerAAA_120,
        WFC_Rotations3D::CornerAAA_240,
        WFC_Rotations3D::CornerABA_120,
        WFC_Rotations3D::CornerABA_240,
        WFC_Rotations3D::CornerBAA_120,
        WFC_Rotations3D::CornerBAA_240,
        WFC_Rotations3D::CornerBBA_120,
        WFC_Rotations3D::CornerBBA_240
    });

    //Put everything into the output collection.
    for (auto rot : buffer_usedRotations)
    {
        output.Add(FWFC_Transform3D(rot));
        if (UseInvertedTransforms)
            output.Add(FWFC_Transform3D(rot, true));
    }
    output.Append(SpecificSupportedTransforms);
}