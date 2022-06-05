# .NET COM Platform Abstraction Layer (DNCP)

A library to support transitioning from [.NET Framework][netfx_download] (Windows only) to a [.NET cross-platform][dotnet_download] environment.

## Requirements

The following are required to consume DNCP.

* [C11](https://en.cppreference.com/w/c/language/history) compatible compiler.
* [CMake](https://cmake.org/download/) &ndash; minimum is 3.6.2.

## Build

1) `cmake -S . -B artifacts`
1) `cmake --build artifacts --target install`

## Test

Unit test the library by running `dncp_test` from the `bin` directory under `artifacts`.

Scenario test the library using `dotnet test`. Run the following command after building the library.

Windows:

`> dotnet test tests\scenario\ComClient`

Non-Windows:

`> dotnet test tests/scenario/ComClient`

<!-- Links -->

[dotnet_download]: https://dotnet.microsoft.com/download
[netfx_download]: https://dotnet.microsoft.com/download/dotnet-framework