# WFCpp

A Visual C++ 2015 library for the [WaveFunctionCollapse](https://github.com/mxgmn/WaveFunctionCollapse) algorithm, plus an optional command-line interface.

For more explanation of the algorithm itself, you can read this project's source or check out [EasyWFC](https://github.com/heyx3/EasyWFC), whose behavior should be identical to this project (except for the PRNG).

The core library has a "Simple" version (for pixel art images), a "tiled" version (for tile-sets), and a "tiled 3D" version (for 3D tile-sets). The first two systems have corresponding command-line programs for playing with them. The 3D tile-set system is more complicated; it uses SWIG to generate a C# interface, and then a Unity3D plugin to facilitate its use.

## Structure

The project has one Visual C++ solution with three projects, plus a Unity3D project:

### WFC++

The actual algorithm code is in the WFC++ project. When built, it creates the .dll, .lib, and .h/.hpp files in the "Build" folder.

The code files are split into the following categories, all of which are inside the `WFC` namespace:

* "Helper Classes": Basic classes that make the code easier to read, and the library itself easier to use. This includes `List`, `Dictionary`, `Array2D`, `Vector2i`, `Region2i`, and `Nullable`.
* "Simple Generator": In the `WFC::Simple` namespace are objects for running the "simple" WFC algorithm, using an input image to generate an output image. The classes are `Pattern`, `InputData`, and `State`. The bulk of the algorithm is in `State`.
* "Tiled Generator": In the `WFC::Tiled` namespace, are objects for running the "tiled" WFC algorithm. It is structured similar to the "Simple" generator, with `InputData` and `State` classes, plus some tile-specific helper classes like `Tile` and `TilePermutator`.
* **(WIP, not finished yet)** "Tiled3D Generator": In the `WFC::Tiled3D` namespace are classes for the 3D version of the "tile" generator. The algorithm itself is a simple extrapolation of the 2D version, but the input data is *much* more complicated. In particular, the ability to auto-generate rotated and flipped versions of the input data gets a lot more complicated in 3D. Whereas the 2D version asks you to assign an ID to each tile's 4 edges and provide some meta-data about tile symmetry and edge symmetry, the 3D version asks you to assign an ID to each *point* on each tile's 6 faces, so that each tile has 6 * 4 = 24 IDs. The symmetries of each face are implicit in these IDs, by giving multiple points the same ID so that the points aren't changed under certain transformations.
* Extraneous files: *HelperClasses.h*, and *Platform.h*.

### WFCmd

The command-line interface for the "Simple" WFC algorithm. Uses standard input/output to get the input image data and various settings for the generator. Refer to *main.cpp* for documentation on how to interact with this program.

If the "-shellMode" flag is passed to the executable, nothing is output except for the final result of running the algorithm. This allows you to easily pipe the output image somewhere, such as another program.

### WFCtile

The command-line interface for the "Tiled" WFC algorithm. Pulls all config data from a folder containing text files. Outputs the result to stdout as either a simple binary format, or as a PGM image file. Refer to *main.cpp* for more documentation on how to interact with this program.

### TODO: Tiled3D