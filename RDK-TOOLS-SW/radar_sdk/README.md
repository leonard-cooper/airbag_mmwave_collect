# Radar SDK

The Radar SDK is an Infineon software product for the development of
applications with Infineon radar devices. It allows to access Infineon 60GHz
radar sensors from C, C++, Python, and Matlab.

## Documentation

The documentation is available in the directory doc/. It includes amon others
an introduction to Radar and the Radar SDK, instructions on how to build the
Radar SDK, as well as tutorials for the Python and Matlab wrapper.

## Supported platforms

Currently, following platforms are officially supported:
- Windows 10: Microsoft Visual Studio 2017 or higher (32bit and 64bit), MinGW
  (gcc, 64bit)
- Linux (x86-64, armhf): gcc, clang

The software has not been tested on other platforms. It might work or it might
not work on unsupported platforms. Note that the segmentation algorithm and
application is only available on officially supported platforms.

For more information please refer to the documentation.

## Compilation

The SDK uses [cmake](https://cmake.org) as build system. On Windows, you can
either download the [latest version of cmake](https://cmake.org/download/) from
the official website, or you can use Visual Studio or MinGW which have cmake
included.

The typical workflow to build the sources is:
  1. Create a new directory `build` in the release package directory.
  2. Run `cmake ..` in this directory or use the cmake GUI.
  3. Compile the sources: Depending on your generator run `ninja` (usually on
     Linux, MinGW), or open the Visual Studio solution.

Further information on building the Radar SDK can be found in the
documentation.

## License

The Radar SDK is licensed under the Infineon Evaluation Software License
Agreement (ESLA). Some parts use more liberal licenses.

| Name                 | License   | directory                  |
|----------------------|-----------|----------------------------|
| argparse             | MIT       | 3rd\_party/libs/argparse   |
| kissfft              | Unlicense | 3rd\_party/libs/kissfft    |
| nlohmann             | MIT       | 3rd\_party/libs/nlohmann   |
| lib\_avian           | MIT       | external/lib\_avian        |
| strata               | IFX ESLA  | external/strata            |
| ifxAlgo              | MIT       | sdk/c/ifxAlgo              |
| ifxBase              | MIT       | sdk/c/ifxBase              |
| ifxDevice            | MIT       | sdk/c/ifxDevice            |
| ifxRadar             | MIT       | sdk/c/ifxRadar             |
| ifxRadarSegmentation | IFX ESLA  | sdk/c/ifxRadarSegmentation |
| C example apps       | MIT       | apps/c                     |
| Python example apps  | MIT       | apps/py/examples           |
| Python wrapper       | MIT       | sdk/py/wrapper             |
| Matlab wrapper       | MIT       | sdk/matlab/wrapper         |

The license text can either be found as a separate file in the respective
directory or at the beginning of the source code.

## Material on the Internet

* [60GHz Radar](https://www.infineon.com/60ghz) - The official product homepage for Infineon's 60GHz Radar.
