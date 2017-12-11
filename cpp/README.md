# Prequisites

Peacock++ requires [ImageMagick](https://www.imagemagick.org)'s
[Magick++](http://www.imagemagick.org/Magick++/), version 7 or higher.
Peacock++ also requires a C++11 compiler.

Peacock++ can be built using CMake, version 3.5 or higher.

Peacock++ has been built and tested on [macOS](https://www.apple.com/macos/)
10.12.6 with Magick++ version 7.0.7-12 (provided via
[Homebrew](https://brew.sh/)).

# C++ Build Instructions

In the `cpp` directory (that you're reading this file in), do the following to
build `peacock` "out of source" using CMake:

1. Create a new directory `build`.
2. Enter the `build` directory and execute CMake on the `CMakeLists.txt` file in
   `src`.
3. Execute `make`.

An example of the commands needed are as follows.

```
mkdir build
cd build
cmake ../src
make
```

If not using CMake, one might instead issue a build command such as the one
shown in `make.sh`.

# Open-source Software Credits

Peacock++ uses the `args.hxx` header-only C++ command line option parser for
command line argument processing.  This was acquired from
[https://github.com/Taywee/args](https://github.com/Taywee/args) master branch
commit 9ae38139a67c9187d85e06cdcce92196a3ff6de7 and renamed to `args.hpp` in
keeping with this project's style.  This file is used under the MIT license (see
top of `args.hpp`).
