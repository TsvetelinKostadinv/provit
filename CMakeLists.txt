cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(provit VERSION 0.0.1)

set(SOURCES
    src/main.cpp
)

SET(HEADERS
    include/parse_result.h
)

source_group("Sources" FILES ${SOURCES})
source_group("Headers" FILES ${HEADERS})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(VS_STARTUP_PROJECT ${PROJECT_NAME})

configure_file(include/provit.h.in provit.h)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_include_directories(${PROJECT_NAME} PRIVATE ./include ${PROJECT_BINARY_DIR} SYSTEM)


if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4) #  /WX
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion) # -Werror
endif()
