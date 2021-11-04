cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(provit)

set(SOURCES
    main.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE SYSTEM)


if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4) #  /WX
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion) # -Werror
endif()