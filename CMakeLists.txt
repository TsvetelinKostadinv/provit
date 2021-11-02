cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(provit)

set(SOURCES
    main.cpp
)

set(VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE SYSTEM)


if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /Wall) #  /WX
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion) # -Werror
endif()