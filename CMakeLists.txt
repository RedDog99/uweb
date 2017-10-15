project(uWeb)

option(UWEB_DEBUG "Enable debug messages" OFF)

if(UWEB_DEBUG)
    add_definitions(-DDEBUG)
endif(UWEB_DEBUG)

set(UWEB_SRC
    main.cpp
    uweb.cpp
    )

add_executable(uweb ${UWEB_SRC})
target_link_libraries(uweb)
