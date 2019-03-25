#pragma once

#include "HelperClasses.h"
#include "Simple/State.h"
#include "Tiled/State.h"


//Exports all templated classes needed for this dll.
//The idea is to have one centralized place for all these exports to exist.

namespace WFC
{
    EXPORT_WFC_LIST(Vector2i);
    EXPORT_WFC_NULLABLE(bool);
    EXPORT_WFC_SET_SELF(Vector2i);

    namespace Simple
    {
        EXPORT_WFC_ARRAY2D(Pixel);
        EXPORT_WFC_LIST(Pattern);
        EXPORT_WFC_DICT_STD(Pixel, size_t);

        EXPORT_WFC_NULLABLE(Pixel);
        EXPORT_WFC_ARRAY2D(State::OutputPixel);


        EXPORT_WFC_DICT_STD(Transformations, Array2D<Pixel>);
    }
    namespace Tiled
    {
        EXPORT_WFC_LIST(Tile);
        EXPORT_WFC_SET_STD(EdgeID);
        EXPORT_WFC_SET_STD(TileID);
        EXPORT_WFC_DICT_STD(EdgeID, EdgeID);

        EXPORT_WFC_NULLABLE(TileID);
        EXPORT_WFC_ARRAY2D(State::OutputTile);
    }
}