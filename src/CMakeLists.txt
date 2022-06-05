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
    DNCP_TYPEDEFS
    DNCP_WINHDRS
  )
endif()

set(HEADERS
  inc/dncp.h
)

add_library(dncp
  STATIC
  ${SOURCES}
  ${HEADERS}
)

set_target_properties(dncp PROPERTIES PUBLIC_HEADER inc/dncp.h)
target_include_directories(dncp PUBLIC ./inc)

if(NOT WIN32)
  # Marked private because consumption of the Windows' headers is
  # a build implementation detail and shouldn't impose on the consumer
  # of dncp.
  target_include_directories(dncp PRIVATE ./inc/winhdrs)
endif()

install(TARGETS dncp)

# Chain in the Windows headers interface
add_subdirectory(inc/winhdrs)
