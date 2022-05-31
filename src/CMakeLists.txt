# Configure the compiler
include(../configure.cmake)

if(WIN32)
    set(SOURCES
        windows.c
        )
else()
    set(SOURCES
        memory.c
        bstr.c
        guids.c
        interfaces.c
        )
    add_compile_definitions(
        DNCOMPAL_TYPEDEFS
        DNCOMPAL_WINHDRS)
endif()

set(HEADERS
    inc/dncompal.h
    )

add_library(dncompal
    STATIC
    ${SOURCES}
    ${HEADERS})

set_target_properties(dncompal PROPERTIES PUBLIC_HEADER inc/dncompal.h)
target_include_directories(dncompal PUBLIC ./inc)

if(NOT WIN32)
    # Marked private because consumption of the Windows' headers is
    # a build implementation detail and shouldn't impose on the consumer
    # of dncompal.
    target_include_directories(dncompal PRIVATE ./inc/winhdrs)
endif()

install(TARGETS dncompal)

# Chain in the Windows headers interface
add_subdirectory(inc/winhdrs)
