# Configure the compiler
include(../configure.cmake)

if(WIN32)
    set(SOURCES
        windows.c
        )
    add_compile_definitions(DNCOMPAL_WINDOWS)
else()
    set(SOURCES
        memory.c
        bstr.c
        guids.c
        interfaces.c
        )
    add_compile_definitions(DNCOMPAL_TYPEDEFS)
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

install(TARGETS dncompal)