#include "TestTilesets.hpp"

TEST(Sanity)
{
    CHECK_EQUAL(1, 1);
}

SUITE(WFC_Simple)
{
    using namespace Tilesets::Simple;
}

SUITE(WFC_Tiled)
{
    using namespace Tilesets::Tiled;
}

SUITE(WFC_Tiled3D)
{
    using namespace Tilesets::Tiled3D;
}


int main(int, const char* [])
{
    return UnitTest::RunAllTests();
}