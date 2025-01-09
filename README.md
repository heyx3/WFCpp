# WFCpp

![A demo of the old Unreal 4 plugin](https://i.imgur.com/vkABeE1.gif)

A C++ library for the amazing [WaveFunctionCollapse](https://github.com/mxgmn/WaveFunctionCollapse) algorithm, plus various utilities for running it.
There is also [an Unreal 5 plugin](https://github.com/heyx3/WfcUnreal) (WIP) to integrate the `Tiled3D` module with Unreal Engine.

For more explanation of the algorithm itself, you can read this project's source or check out [EasyWFC](https://github.com/heyx3/EasyWFC), whose behavior should be identical to this project (except for the PRNG).

The core library has a "Simple" version (for pixel art images), a "tiled" version (for tile-sets), and a "tiled 3D" version (for 3D tile-sets).

The first two systems have corresponding command-line programs for playing with them.
The 3D tile-set system is more complex and only offers Unreal Engine 5 integration.

## Setup

After cloning the repo, just open **WFC++.sln** in Visual Studio, set the platform to **x64**, and build. You may want to change the "startup project" to one of the command-line apps, if that's what you want to use.

## Structure

The project has one Visual C++ solution with four projects. The Unreal Engine plugin is kept in a different repo (linked above).

### WFC++

The *WFC++* project contains the actual algorithm code.
When built (apart from Unreal) it creates the .dll, .lib, and .h/.hpp files in the "Build" folder.

The code files are split into the following categories, all of which are inside the `WFC` namespace:

* "Helper Classes": Basic classes that make the code easier to read, and the library itself easier to use. This includes `Array2D`, `Vector2i`, and `Region2i`.
* "Simple Generator": In the `WFC::Simple` namespace are objects for running the "simple" WFC algorithm, using an input image to generate an output image. The classes are `Pattern`, `InputData`, and `State`. The bulk of the algorithm is in `State`.
* "Tiled Generator": In the `WFC::Tiled` namespace, are objects for running the "tiled" WFC algorithm. It is structured similar to the "Simple" generator, with `InputData` and `State` classes, plus some tile-specific helper classes like `Tile` and `TilePermutator`.
* "Tiled3D Generator": In the `WFC::Tiled3D` namespace are classes for the 3D version of the "tile" generator. The algorithm itself is a simple extrapolation of the 2D version, but the input data is *much* more complicated. In particular, the ability to auto-generate rotated and flipped versions of the input data gets a lot more complicated in 3D. Whereas the 2D version asks you to assign an ID to each tile's 4 edges and provide some meta-data about tile symmetry and edge symmetry, the 3D version asks you to assign an ID to each *point* on each tile's 6 faces, giving each tile 6 * 4 = 24 IDs. The symmetries of each face are implicitly defined by giving multiple points the same ID so that the face isn't changed under certain transformations.
* Extraneous files: *HelperClasses.h*, and *Platform.h*.

### WFCmd

The command-line interface for the "Simple" WFC algorithm. Uses standard input/output to get the input image data and various settings for the generator. Refer to *main.cpp* for documentation on how to interact with this program.

If the "-shellMode" flag is passed to the executable, nothing is output except for the final result of running the algorithm. This allows you to easily pipe the output image somewhere, such as another program.

### WFCtile

The command-line interface for the "Tiled" WFC algorithm. Pulls all config data from a folder containing text files. Outputs the result to stdout as either a simple binary format, or as a PGM image file. Refer to *main.cpp* for more documentation on how to interact with this program.

## Tests

Unit tests use the *UnitTest++*"* library, kept in the folder 'UnitTestLibrary'.
I tried to make it a git submodule, but they're quite annoying to work with so the library is pasted right into the repo.

The actual unit tests are in the project *WFCtests*.
Just run this project to run the unit tests.

## License

MIT license; go crazy.

Copyright 2017-2025 William A. Manning

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
