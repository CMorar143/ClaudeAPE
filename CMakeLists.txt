cmake_minimum_required(VERSION 3.20)
project(TreeSwinging CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(SDL_SHARED ON CACHE BOOL "" FORCE)
set(SDL_STATIC OFF CACHE BOOL "" FORCE)
set(SDL_TEST_LIBRARY OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    SDL2
    URL https://github.com/libsdl-org/SDL/releases/download/release-2.30.9/SDL2-2.30.9.zip
)
FetchContent_MakeAvailable(SDL2)

add_executable(TreeSwinging
    src/main.cpp
    src/Game.cpp
    src/Renderer.cpp
    src/InputManager.cpp
)

target_include_directories(TreeSwinging PRIVATE src)
target_link_libraries(TreeSwinging PRIVATE SDL2::SDL2 SDL2::SDL2main)

if (WIN32)
    add_custom_command(TARGET TreeSwinging POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL2::SDL2>
            $<TARGET_FILE_DIR:TreeSwinging>
    )
endif()
