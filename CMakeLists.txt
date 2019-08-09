
cmake_minimum_required(VERSION 3.4.1)
project(spooky)

find_library(
    allegro
    NAMES allegro
    )

set(LIBS
  liballeg.so
)

add_executable(
    spooky
    
    src/main.cc
)

target_link_libraries(
    spooky

    ${LIBS}
)

install(
    TARGETS
    spooky
    DESTINATION
    bin
    )

