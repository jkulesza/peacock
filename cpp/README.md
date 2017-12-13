# How to use it?

This script is used to convert "normally" colored images to a representation
similar to what a colorblind viewer might see.   The types of colorblindness
that can be simulated are:

* Normal (normal vision)
* Protanopia (red-blind)
* Deuteranopia (green-blind)
* Tritanpoia (blue-blind)
* Protanomaly (red-weak)
* Deuteranomaly (green-weak)
* Tritanomaly (blue-weak)
* Monochromacy (totally colorblind)

with the default action to convert to 'All' types of colorblindness (and to a
normal vision version).  Converting to only a select type of colorblindness can
be accomplished with the `c` parameter described below.

The conversion processes and coefficients herein are used with permission from
Colblindor [http://www.color-blindness.com/] and were therein used with
permission of Matthew Wickline and the Human-Computer Interaction Resource
Network [http://www.hcirn.com/] for non-commercial purposes.  As such, this code
may only be used for non-commercial purposes.

```
positional arguments:
  inputimage  input image file name

optional arguments:
  -h, --help                        Display help menu
  -c[CBTYPE],
  --colorblindness=[CBTYPE]         The type of colorblindness, see below
                                    for valid entries (if not used: all)
```

Typical command line calls might look like:

`./peacock <inputimage>`

or

`./peacock -cProtanopia <inputimage>`


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

# Open-source Software Credits<a name="OSSCredits"></a>

Peacock++ uses the `args.hxx` header-only C++ command line option parser for
command line argument processing.  This was acquired from
[https://github.com/Taywee/args](https://github.com/Taywee/args) master branch
commit 9ae38139a67c9187d85e06cdcce92196a3ff6de7 and renamed to `args.hpp` in
keeping with this project's style.  This file is used under the MIT license (see
top of `args.hpp`).
