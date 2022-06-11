# .NET COM Platform Abstraction Layer

A library to support porting C/C++ COM binaries from [.NET Framework][netfx_download] (Windows only) to a [.NET cross-platform][dotnet_download] environment.

## Requirements

The following are required to consume DNCP.

* [C11](https://en.cppreference.com/w/c/language/history) compatible compiler.
* [CMake](https://cmake.org/download/) &ndash; minimum is 3.6.2.

## Build

The root of the project contains a `dncp.proj` file that should be used to build. Directly using CMake is possible but since it is assume the .NET SDK is installed on the system it is recommended to use the `dotnet` command. The common `build`, `clean` and `test` commands are supported. For example, `dotnet build` will build the project.

## Test

Unit and scenario test the library using `dotnet test`.

## Usage

The recommended way to use DNCP is via [git submodules][git_submodules] and CMake.

The [`dncp.h`](./src/inc/dncp.h) header contains consistent definitions of common COM APIs. All data types are identically named to those in official Windows' headers. All functions are prefixed with `PAL_` followed by the official Win32 name (for example, `SysAllocString` is `PAL_SysAllocString`). Ideally, all functions provided by DNCP replace the corresponding Win32 APIs as DNCP will forward to the Windows implementation when running on Windows &ndash; see [`windows.c`](./src/windows.c).

Users can set defines during compilation to tailor how they would like to develop their COM library.

* `DNCP_TYPEDEFS` &ndash; Defines the most common Win32 data types needed for .NET scenarios. If this isn't set, the project should define them or include the official Windows' headers.

* `DNCP_WINHDRS` &ndash; Includes the minimal mocked out Windows' headers provided by DNCP. This is typically needed if building on non-Windows and/or referencing official .NET headers (for example, [`cor.h`](https://github.com/dotnet/runtime/blob/main/src/coreclr/inc/cor.h)). If this is set, the project should also link against `dncp_winhdrs`, see example below.

### Walkthrough

These steps are the general process. Note that depending on how complex your project is, these may not be sufficient.

1. Add DNCP as a submodule to your repository.
    - `git submodule add https://github.com/AaronRobinsonMSFT/DNCP.git`

1. Add a reference to the DNCP directory in a CMake build file.
    ```cmake
    add_subdirectory(DNCP/)
    ```

1. Reference the `dncp.h` header and link the DNCP static library into the target project.
    ```c
    #include <dncp.h>
    ```

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

## FAQs

1. The implementation of some string functions don't check for `NULL` inputs, this makes the APIs less robust. Why don't they check for `NULL`?
    - DNCP is designed to be a drop in replacement for the APIs on Win32. The intent is to match semantics identically. This is reflected in the unit tests when they run on Windows.

<!-- Links -->

[dotnet_download]: https://dotnet.microsoft.com/download
[netfx_download]: https://dotnet.microsoft.com/download/dotnet-framework
[git_submodules]: https://git-scm.com/book/en/v2/Git-Tools-Submodules
