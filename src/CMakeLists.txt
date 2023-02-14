# Configure the compiler
include(../configure.cmake)

if(WIN32)
  set(SOURCES
    windows.c
  )
else()
  set(SOURCES
    bstr.c
    guids.c
    interfaces.c
    memory.c
    strings.c
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

add_library(dncp::dncp ALIAS dncp)

set_target_properties(dncp PROPERTIES PUBLIC_HEADER inc/dncp.h)
target_include_directories(dncp PRIVATE ./inc)

if(NOT WIN32)
  # Marked private because consumption of the Windows' headers is
  # a build implementation detail and shouldn't impose on the consumer
  # of dncp.
  target_include_directories(dncp PRIVATE ./inc/winhdrs)
endif()

# Chain in the Windows headers interface
add_subdirectory(inc/winhdrs)

install(TARGETS dncp EXPORT dncp
  PUBLIC_HEADER DESTINATION include
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

install(EXPORT dncp NAMESPACE dncp:: FILE dncplib.cmake DESTINATION lib/cmake/dncp)

include(CMakePackageConfigHelpers)
configure_package_config_file(config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/dncp-config.cmake
  INSTALL_DESTINATION lib/cmake/dncp)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/dncp-config-version.cmake
  VERSION 1.0.0
  COMPATIBILITY SameMajorVersion )
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/dncp-config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/dncp-config-version.cmake
        DESTINATION lib/cmake/dncp)
