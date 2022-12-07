
cmake_minimum_required(VERSION 3.4.1)
project(spooky)




add_executable(
    spooky
    
    src/main.cc
)


install(
    TARGETS
    spooky
    DESTINATION
    bin
    )

