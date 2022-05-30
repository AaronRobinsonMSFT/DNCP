set(SOURCES
    comserver.cpp
    )

add_library(comserver
    SHARED
    ${SOURCES}
    ${HEADERS})

if(NOT WIN32)
    add_compile_definitions(
        DNCOMPAL_TYPEDEFS
        DNCOMPAL_WINHDRS)

    # Include the exported headers for non-Windows building.
    target_link_libraries(comserver dncompal_winhdrs)
endif()

target_link_libraries(comserver dncompal)
install(TARGETS comserver)