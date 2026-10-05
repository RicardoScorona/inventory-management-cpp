cmake_minimum_required(VERSION 3.20)

project(
    InventoryManagement
    VERSION 0.2.0
    DESCRIPTION "Modern C++ inventory management system"
    LANGUAGES CXX
)

# --------------------------------------------------
# C++ configuration
# --------------------------------------------------

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --------------------------------------------------
# External dependencies
# --------------------------------------------------

find_package(SQLite3 REQUIRED)

# --------------------------------------------------
# Core library
# --------------------------------------------------

add_library(
    inventory_core

    src/models/Product.cpp
    src/services/InventoryService.cpp
    src/repositories/SQLiteProductRepository.cpp
)

target_include_directories(
    inventory_core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(
    inventory_core
    PRIVATE
        SQLite::SQLite3
)

# --------------------------------------------------
# Compiler warnings
# --------------------------------------------------

if(MSVC)
    target_compile_options(
        inventory_core
        PRIVATE
            /W4
            /permissive-
    )
else()
    target_compile_options(
        inventory_core
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()

# --------------------------------------------------
# Main application
# --------------------------------------------------

add_executable(
    inventory_management
    src/main.cpp
)

target_link_libraries(
    inventory_management
    PRIVATE
        inventory_core
)

# --------------------------------------------------
# Testing dependencies
# --------------------------------------------------

include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)

if(MSVC)
    set(
        gtest_force_shared_crt
        ON
        CACHE BOOL ""
        FORCE
    )
endif()

FetchContent_MakeAvailable(googletest)

enable_testing()

# --------------------------------------------------
# Product tests
# --------------------------------------------------

add_executable(
    ProductTests
    tests/ProductTests.cpp
)

target_link_libraries(
    ProductTests
    PRIVATE
        inventory_core
        GTest::gtest_main
)

# --------------------------------------------------
# InventoryService tests
# --------------------------------------------------

add_executable(
    InventoryServiceTests
    tests/InventoryServiceTests.cpp
)

target_link_libraries(
    InventoryServiceTests
    PRIVATE
        inventory_core
        GTest::gtest_main
)

# --------------------------------------------------
# SQLite repository integration tests
# --------------------------------------------------

add_executable(
    SQLiteProductRepositoryTests
    tests/SQLiteProductRepositoryTests.cpp
)

target_link_libraries(
    SQLiteProductRepositoryTests
    PRIVATE
        inventory_core
        GTest::gtest_main
)

# --------------------------------------------------
# Test discovery
# --------------------------------------------------

include(GoogleTest)

gtest_discover_tests(ProductTests)
gtest_discover_tests(InventoryServiceTests)
gtest_discover_tests(SQLiteProductRepositoryTests)
