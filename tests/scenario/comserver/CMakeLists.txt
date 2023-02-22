# COM server that is loadable in a .NET runtime on any platform.

set(SOURCES
  comserver.cpp
)

add_library(comserver
  SHARED
  ${SOURCES}
  ${HEADERS}
)

if(NOT WIN32)
  # Include the exported headers for non-Windows building.
  target_link_libraries(comserver dncp::winhdrs)
endif()

target_link_libraries(comserver dncp::dncp)
install(TARGETS comserver
  DESTINATION bin
)