#pragma once

//This file is not included by default, to keep compile and binary size down.
//But if you want to be able to print Tile3D structures to somewhere like cout,
//    include this file.

#include "Simple/State.h"
#include "Tiled/State.h"
#include "Tiled3D/State.h"

#include <iomanip>
#include <format>
#include <ostream>


inline std::ostream& operator<<(std::ostream& cout, const WFC::Vector2i& v)
{
    return cout << '{' << v.x << ", " << v.y << '}';
}
inline std::ostream& operator<<(std::ostream& cout, const WFC::Vector3i& v)
{
    return cout << '{' << v.x << ", " << v.y << ", " << v.z << '}';
}
inline std::ostream& operator<<(std::ostream& cout, const WFC::Vector4i& v)
{
    return cout << '{' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << '}';
}

inline std::ostream& operator<<(std::ostream& cout, WFC::Tiled3D::FacePoints p)
{
    using namespace WFC::Tiled3D;

    switch (p)
    {
        case AA: return (cout << "AA");
        case AB: return (cout << "AB");
        case BA: return (cout << "BA");
        case BB: return (cout << "BB");
        default: assert(false); return (cout << "?" << (int)p << "");
    }
}
inline std::ostream& operator<<(std::ostream& cout, WFC::Tiled3D::Directions3D d)
{
    using namespace WFC::Tiled3D;

    switch (d)
    {
        case MinX: return (cout << "-X");
        case MaxX: return (cout << "+X");
        case MinY: return (cout << "-Y");
        case MaxY: return (cout << "+Y");
        case MinZ: return (cout << "-Z");
        case MaxZ: return (cout << "+Z");
        default: assert(false); return (cout << "?" << (int)d << "");
    }
}
inline std::ostream& operator<<(std::ostream& cout, WFC::Tiled3D::Rotations3D r)
{
    using namespace WFC::Tiled3D;

    switch (r)
    {
        case Rotations3D::None: return (cout << "None");
        case Rotations3D::AxisX_90: return (cout << "AxisX_90");
        case Rotations3D::AxisX_180: return (cout << "AxisX_180");
        case Rotations3D::AxisX_270: return (cout << "AxisX_270");
        case Rotations3D::AxisY_90: return (cout << "AxisY_90");
        case Rotations3D::AxisY_180: return (cout << "AxisY_180");
        case Rotations3D::AxisY_270: return (cout << "AxisY_270");
        case Rotations3D::AxisZ_90: return (cout << "AxisZ_90");
        case Rotations3D::AxisZ_180: return (cout << "AxisZ_180");
        case Rotations3D::AxisZ_270: return (cout << "AxisZ_270");
        case Rotations3D::EdgesXa: return (cout << "EdgesXa");
        case Rotations3D::EdgesXb: return (cout << "EdgesXb");
        case Rotations3D::EdgesYa: return (cout << "EdgesYa");
        case Rotations3D::EdgesYb: return (cout << "EdgesYb");
        case Rotations3D::EdgesZa: return (cout << "EdgesZa");
        case Rotations3D::EdgesZb: return (cout << "EdgesZb");
        case Rotations3D::CornerAAA_120: return (cout << "CornerAAA_120");
        case Rotations3D::CornerAAA_240: return (cout << "CornerAAA_240");
        case Rotations3D::CornerABA_120: return (cout << "CornerABA_120");
        case Rotations3D::CornerABA_240: return (cout << "CornerABA_240");
        case Rotations3D::CornerBAA_120: return (cout << "CornerBAA_120");
        case Rotations3D::CornerBAA_240: return (cout << "CornerBAA_240");
        case Rotations3D::CornerBBA_120: return (cout << "CornerBBA_120");
        case Rotations3D::CornerBBA_240: return (cout << "CornerBBA_240");
        default: assert(false); return (cout << "?" << (int)r << "");
    }
}
inline std::ostream& operator<<(std::ostream& cout, WFC::Tiled3D::Transform3D tr)
{
    if (tr.Invert)
        cout << "Invert=>" << tr.Rot;
    else
        cout << tr.Rot;

    return cout;
}
inline std::ostream& operator<<(std::ostream& cout, WFC::Tiled3D::TransformSet set)
{
    cout << "0x" << std::format("{:x}", set.Bits());
    cout << "{ ";

    int i = 0;
    for (WFC::Tiled3D::Transform3D tr : set)
    {
        if (i != 0)
            cout << ", ";
        i += 1;

        cout << tr;
    }

    if (set.Size() > 0)
        cout << ' ';
    cout << '}';

    return cout;
}