cmake_minimum_required(VERSION 3.20)

project(
    InventoryManagement
    VERSION 0.1.0
    DESCRIPTION "Modern C++ inventory management system"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(
    inventory_management

    src/main.cpp
    src/models/Product.cpp
    src/services/InventoryService.cpp
)

target_include_directories(
    inventory_management
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

if(MSVC)
    target_compile_options(
        inventory_management
        PRIVATE
            /W4
            /permissive-
    )
else()
    target_compile_options(
        inventory_management
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()
