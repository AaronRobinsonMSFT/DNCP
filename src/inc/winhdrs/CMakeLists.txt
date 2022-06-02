# Customized Windows headers needed to consume COM headers in a .NET scenario.
# These are copies or modifications of MIT licensed headers. See each
# headers for license details.

set(WINHDRS_HEADERS
    objidl.h
    ole2.h
    poppack.h
    pshpack1.h
    rpc.h
    rpcndr.h
    specstrings.h
    unknwn.h
    winerror.h)

add_library(dncp_winhdrs INTERFACE ${WINHDRS_HEADERS})
target_include_directories(dncp_winhdrs INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:include/winhdrs>
)

install(FILES ${WINHDRS_HEADERS}
  DESTINATION include/winhdrs
)