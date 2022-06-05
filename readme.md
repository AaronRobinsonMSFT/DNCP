# COM Platform Abstraction Layer for .NET

A library to support porting C/C++ COM binaries from [.NET Framework][netfx_download] (Windows only) to a [.NET cross-platform][dotnet_download] environment.

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

## Usage

The recommended way to use DNCP is via [git submodules][git_submodules] and CMake.

The `dncp.h` header contains uniform definitions of common COM APIs. All types are identically named to those in official Windows headers. All functions are prefixed with `PAL_` followed by their official Win32 name (for example, `SysAllocString` is `PAL_SysAllocString`). Ideally, all instances of the functions provided by DNCP will replace explicit uses of the Win32 APIs as DNCP will properly forward to the Windows implementation when running on Windows &ndash; see [`windows.c`](./src/windows.c).

Users can set some defines during compilation to tailor how they would like to develop their COM library.

* `DNCP_TYPEDEFS` &ndash; Defines the most common Win32 types needed for .NET scenarios. If this isn't set, the project should define them or include the official Windows' headers.

* `DNCP_WINHDRS` &ndash; Includes the minimal mocked out Windows' headers provided by DNCP. This is typically needed if building on non-Windows and/or referencing official .NET headers (for example, [`cor.h`](https://github.com/dotnet/runtime/blob/main/src/coreclr/inc/cor.h)). If this is set, the project should also link against `dncp_winhdrs`, see example below.

### Walkthrough

These steps are the general process. Note that depending on how complex your project is, these may not be sufficient.

1. Add DNCP as a submodule to your repository.
    - `git submodule add https://github.com/AaronRobinsonMSFT/DNCP.git`

1. Add a reference to the DNCP directory in a CMake build file.
    ```cmake
    add_subdirectory(DNCP/)
    ```

1. Reference the [`dncp.h`](./src/inc/dncp.h) header and link the DNCP static library into the target project.
    ```cmake
    target_link_libraries(mycomlib dncp)
    ```

1. For non-Windows build or if you are trying to avoid referencing any Windows header files, the following defines should be set.
    ```cmake
    add_compile_definitions(
        DNCP_TYPEDEFS
        DNCP_WINHDRS
    )

    target_link_libraries(mycomlib dncp_winhdrs)
    ```

An example of consumption can be found in the [scenario test project](./tests/scenario/comserver/CMakeLists.txt).

<!-- Links -->

[dotnet_download]: https://dotnet.microsoft.com/download
[netfx_download]: https://dotnet.microsoft.com/download/dotnet-framework
[git_submodules]: https://git-scm.com/book/en/v2/Git-Tools-Submodules