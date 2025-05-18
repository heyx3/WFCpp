# WFCpp

![A demo of the old Unreal 4 plugin, now replaced with a better Unreal 5 one!](https://i.imgur.com/vkABeE1.gif)

A C++ library for the amazing [WaveFunctionCollapse](https://github.com/mxgmn/WaveFunctionCollapse) algorithm, plus various utilities for running it.
There is also [an Unreal 5 plugin](https://github.com/heyx3/WfcUnreal) to integrate the `Tiled3D` module with Unreal Engine.

For more explanation of the algorithm itself, you can read this project's source or check out [EasyWFC](https://github.com/heyx3/EasyWFC),
    which is designed to be more educational and interactive.

The core library has a "Simple" version (for pixel art images), a "tiled" version (for tile-sets),
    and a "tiled 3D" version (for 3D tile-sets).

The simpler 2D modules ("Simple" and "Tiled") come with little command-line programs to run them.
I currently am not focusing on them, but they should be perfectly functional already.
Tiled3D, on the other hand, is very complex -- performance has much higher priority
    and there are 48 permutations of a cube tile compared to only 8 for a square tile!
So I have a work-in-progress Unreal 5 plugin to help you wield Tiled3D.

## Setup

After cloning the repo, just open **WFC++.sln** in Visual Studio, set the platform to **x64**, and build.
You should change the "startup project" to one of the command-line apps, or to the unit test project.

## Structure

The project has one Visual C++ solution with four projects. The Unreal Engine plugin is kept in a different repo (linked above).

### WFC++

The *WFC++* project contains the actual algorithm code.
When built (outside of Unreal) it creates the .dll, .lib, and .h/.hpp files in the "Build" folder.

The code files are split into the following categories, all of which are inside the `WFC` namespace:

* "Helper Classes": Basic classes that make the code easier to read, and the library itself easier to use. This includes `Array2D`, `Vector2i`, and `Region2i`.
* "Simple Generator": In the `WFC::Simple` namespace are objects for running the "simple" WFC algorithm, 
using an input image to generate an output image.
The classes are `Pattern`, `InputData`, and `State`.
The bulk of the algorithm is in `State`.
* "Tiled Generator": In the `WFC::Tiled` namespace, are objects for running the "tiled" WFC algorithm.
It is structured similar to the "Simple" generator, with `InputData` and `State` classes
plus some tile-specific helpers like `Tile` and `TilePermutator`.
* "Tiled3D Generator": In the `WFC::Tiled3D` namespace are classes for the 3D version of the "tile" generator.
The algorithm is a simple extrapolation of the 2D version, but the input data is *much* more complicated.
In particular the description of a tile's symmetries gets a lot more complicated, to facilitate automatic generation of rotated and flipped versions of each tile.
Whereas the 2D version asks you to assign an ID to each edge of each square tile, and provide some meta-data about tile symmetry and edge symmetry,
the 3D version asks you to assign an ID to each corner and edge on each of the tile's 6 faces.
That's 6 * 4 = 24 IDs per tile!
The symmetries of each tile face are implicit in these point ID's;
if you give multiple points the same ID then certain transformations will not change the face,
and so the tile is considered symmetric under that transformation and along that face.
* Finally, there are some extraneous project files: *HelperClasses.h*, and *Platform.h*.

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
