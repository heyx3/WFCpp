# WFCpp

A Visual C++ 2015 library for the [WaveFunctionCollapse](https://github.com/mxgmn/WaveFunctionCollapse) algorithm, plus an optional command-line interface.

For more explanation of the algorithm itself, you can read this project's source or check out [EasyWFC](https://github.com/heyx3/EasyWFC), whose behavior should be identical to this project (except for the PRNG).

The core library has both a "Simple" version (for pixel art images) and a "tiled" version (for tile-sets).

## Structure

The Visual C++ solution is split into three projects:

### WFC++

The actual algorithm code is in the WFC++ project. When built, it creates the .dll, .lib, and .h/.hpp files in the "Build" folder.

The code files are split into the following categories, all of which are inside the `WFC` namespace:

* "Helper Classes": Basic classes that make the code easier to read, and the library itself easier to use. This includes `List`, `Dictionary`, `Array2D`, `Vector2i`, `Region2i`, and `Nullable`.
* "Simple Generator": In the `WFC::Simple` namespace, are objects for running the "simple" WFC algorithm: `Pattern`, `InputData`, and `State`. The bulk of the algorithm is in `State`.
* "Tiled Generator": In the `WFC::Tiled` namespace, are objects for running the "tiled" WFC algorithm. It is structured similar to the "Simple" generator, with `InputData` and `State` classes, plus some tile-specific helper classes like `Tile` and `TilePermutator`.
* Extraneous files: *HelperClasses.h*, and *Platform.h*.

### WFCmd

The command-line interface for the "Simple" WFC algorithm. Uses standard input/output to get the input image data and various settings for the generator. Refer to *main.cpp* for documentation on how to interact with this program.

If the "-shellMode" flag is passed to the executable, nothing is output except for the final result of running the algorithm. This allows you to easily pipe the output image somewhere, such as another program.

### WFCtile

The command-line interface for the "Tiled" WFC algorithm. Pulls all config data from a folder containing text files. Outputs the result to stdout as either a simple binary format, or as a PGM image file. Refer to *main.cpp* for more documentation on how to interact with this program.