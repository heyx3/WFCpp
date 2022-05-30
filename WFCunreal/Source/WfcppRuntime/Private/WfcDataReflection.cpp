#include "WfcDataReflection.h"

FTransform FWFC_Transform3D::ToFTransform() const
{
    FQuat rot;
    switch (Rot)
    {
        #define WFC_CASE(enumRot, axisX, axisY, axisZ, angleDegrees) \
            case WFC_Rotations3D::enumRot: \
                rot = FQuat(FVector(axisX, axisY, axisZ).GetUnsafeNormal(), \
                            angleDegrees / 360.0f * PI); \
            break
        
        WFC_CASE(None,  1, 0, 0,  0);
        WFC_CASE(AxisX_90,  1, 0, 0,  90);
        WFC_CASE(AxisX_180,  1, 0, 0,  180);
        WFC_CASE(AxisX_270,  1, 0, 0,  270);
        WFC_CASE(AxisY_90,  0, 1, 0,  90);
        WFC_CASE(AxisY_180,  0, 1, 0,  180);
        WFC_CASE(AxisY_270,  0, 1, 0,  270);
        WFC_CASE(AxisZ_90,  0, 0, 1,  90);
        WFC_CASE(AxisZ_180,  0, 0, 1,  180);
        WFC_CASE(AxisZ_270,  0, 0, 1,  270);
        WFC_CASE(EdgesXa,  0, 1, 1,  180);
        WFC_CASE(EdgesXb,  0, 1, -1,  180);
        WFC_CASE(EdgesYa,  1, 0, 1,  180);
        WFC_CASE(EdgesYb,  1, 0, -1,  180);
        WFC_CASE(EdgesZa,  1, 1, 0,  180);
        WFC_CASE(EdgesZb,  1, -1, 0,  180);
        WFC_CASE(CornerAAA_120, 1, 1, 1, 120);
        WFC_CASE(CornerAAA_240, 1, 1, 1, 240);
        WFC_CASE(CornerBAA_120, -1, 1, 1, 120);
        WFC_CASE(CornerBAA_240, -1, 1, 1, 240);
        WFC_CASE(CornerABA_120, 1, -1, 1, 120);
        WFC_CASE(CornerABA_240, 1, -1, 1, 240);
        WFC_CASE(CornerBBA_120, -1, -1, 1, 120);
        WFC_CASE(CornerBBA_240, -1, -1, 1, 240);

        default: check(false); return FTransform();
    }

    auto scale = FVector::OneVector * (Invert ? -1 : 1);
    
    return FTransform(rot, FVector::ZeroVector, scale);
}
