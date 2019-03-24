#pragma once

#include "HelperClasses.h"
#include "Simple/State.h"
#include "Tiled/InputData.h"


//Exports all templated classes needed for this dll.
//The idea is to have one centralized place for all these exports to exist.

namespace WFC
{
    EXPORT_WFC_LIST(Vector2i);
    EXPORT_WFC_NULLABLE(bool);

    namespace Simple
    {
        EXPORT_WFC_NULLABLE(Pixel);

        EXPORT_WFC_ARRAY2D(State::OutputPixel);
        EXPORT_WFC_ARRAY2D(Pixel);

        EXPORT_WFC_LIST(Pattern);

        EXPORT_WFC_DICT(Transformations, Array2D<Pixel>);
        EXPORT_WFC_DICT(Pixel, size_t);
    }
    namespace Tiled
    {
        EXPORT_WFC_LIST(Tile);
        EXPORT_WFC_SET(const Tile*);
        EXPORT_WFC_DICT(EdgeID, bool);
    }
}