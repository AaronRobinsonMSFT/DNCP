set(SOURCES
    comserver.cpp
    )

add_library(comserver
    SHARED
    ${SOURCES}
    ${HEADERS})

if(NOT WIN32)
    add_compile_definitions(
        DNCP_TYPEDEFS
        DNCP_WINHDRS)

    # Include the exported headers for non-Windows building.
    target_link_libraries(comserver dncp_winhdrs)
endif()

target_link_libraries(comserver dncp)
install(TARGETS comserver)