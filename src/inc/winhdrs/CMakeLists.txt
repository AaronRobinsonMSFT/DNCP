# Customized Windows headers needed to consume COM headers in a .NET scenario.
# These are copies or modifications of MIT licensed headers. See each
# headers for license details.

set(WINHDRS_HEADERS
    oaidl.h
    objidl.h
    ole2.h
    poppack.h
    pshpack1.h
    rpc.h
    rpcndr.h
    specstrings.h
    unknwn.h
    winerror.h)

add_library(winhdrs INTERFACE)
add_library(dncp::winhdrs ALIAS winhdrs)
target_include_directories(winhdrs INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/winhdrs>
)

target_compile_definitions(winhdrs INTERFACE
  DNCP_TYPEDEFS
  DNCP_WINHDRS)

install(FILES ${WINHDRS_HEADERS}
  DESTINATION include/winhdrs
)

install(TARGETS winhdrs EXPORT winhdrs INCLUDES DESTINATION include/winhdrs)

install(EXPORT winhdrs NAMESPACE dncp:: FILE winhdrs.cmake DESTINATION lib/cmake/dncp)