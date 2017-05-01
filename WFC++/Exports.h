#pragma once

#include "HelperClasses.h"
#include "State.h"


//Exports all templated classes needed for this dll.
//The goal is to have one centralized place for all these exports to exist.

namespace WFC
{
    EXPORT_WFC_NULLABLE(bool);
    EXPORT_WFC_NULLABLE(Pixel);

    EXPORT_WFC_ARRAY2D(State::OutputPixel);
    EXPORT_WFC_ARRAY2D(Pixel);

    EXPORT_WFC_LIST(Vector2i);
    EXPORT_WFC_LIST(Pattern);

    EXPORT_WFC_DICT(Transformations, Array2D<Pixel>);
    EXPORT_WFC_DICT(Pixel, size_t);
}