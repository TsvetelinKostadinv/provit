cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(provit)

set(SOURCES
    main.cpp
)

set(VS_STARTUP_PROJECT ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE .)