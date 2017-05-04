# WFCpp

A Visual C++ 2015 library for the [WaveFunctionCollapse](https://github.com/mxgmn/WaveFunctionCollapse) algorithm, plus an optional command-line interface.

For more explanation of the algorithm itself, you can read this file's source or check out [EasyWFC](https://github.com/heyx3/EasyWFC), whose behavior should be identical to this project except for the PRNG.

## Structure

The Visual C++ solution is split into two projects:

### WFC++

The actual algorithm code is in the WFC++ project. When built, it creates the .dll, .lib, and .h/.hpp files in the "Build" folder.

The code files are split into three categories, all of which are inside the `WFC` namespace:

* "Helper Classes": Basic classes that aren't a fundamental part of the WFC algorithm, but do make things much easier. This includes `List`, `Dictionary`, `Vector2i`, `Region2i`, and `Nullable`.
* "Generator Classes": The actual objects that run the WFC algorithm: `Pattern`, `InputData`, and `State`. The bulk of the algorithm is in `State`.
* Extraneous files: *Exports.h*, *HelperClasses.h*, and *Platform.h*.

### WFCmd

The command-line interface. Uses standard input/output to get the input image data and various settings for the generator. Refer to *main.cpp* for documentation on how to interact with this program.

If the "-shellMode" flag is passed to the executable, nothing is output except for the final result of running the algorithm. This allows you to easily pipe the output image somewhere, such as another program.
