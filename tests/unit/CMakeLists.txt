set(SOURCES
    main.cpp
    )

add_executable(dncp_test
    ${SOURCES}
    ${HEADERS})

if(NOT WIN32)
    add_compile_definitions(
        DNCP_TYPEDEFS
        DNCP_WINHDRS)

    # Include the exported headers for non-Windows building.
    target_link_libraries(dncp_test dncp_winhdrs)
endif()

target_link_libraries(dncp_test dncp)
install(TARGETS dncp_test)