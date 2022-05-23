# Configure the compiler
include(../configure.cmake)

if(NOT WIN32)
    add_compile_definitions(DNCOMPAL_TYPEDEFS)
endif()

set(SOURCES
    main.c
    )

add_executable(dncompal_test
    ${SOURCES}
    ${HEADERS})

target_link_libraries(dncompal_test dncompal)
install(TARGETS dncompal_test)