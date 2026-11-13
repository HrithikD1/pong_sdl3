cmake_minimum_required(VERSION 3.20)
project(pong CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SDL3 package installed via Homebrew
find_package(SDL3 REQUIRED)
find_package(SDL3_ttf REQUIRED)

add_executable(pong main.cpp game.cpp)

# Link SDL3 target to your executable
target_link_libraries(pong PRIVATE SDL3::SDL3 SDL3_ttf::SDL3_ttf)