set(SOURCES
    main.cpp
    )

add_executable(dncompal_test
    ${SOURCES}
    ${HEADERS})

if(NOT WIN32)
    add_compile_definitions(
        DNCOMPAL_TYPEDEFS
        DNCOMPAL_WINHDRS)

    # Include the exported headers for non-Windows building.
    target_link_libraries(dncompal_test dncompal_winhdrs)
endif()

target_link_libraries(dncompal_test dncompal)
install(TARGETS dncompal_test)