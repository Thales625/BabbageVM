cmake_minimum_required(VERSION 3.20)
project(BabbageVM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- INCLUDE PATHS ---
include_directories(
    ${PROJECT_SOURCE_DIR}/ui/include/imgui
    ${PROJECT_SOURCE_DIR}/ui/include/SDL2
    ${PROJECT_SOURCE_DIR}/assembler
    ${PROJECT_SOURCE_DIR}/linker
    ${PROJECT_SOURCE_DIR}/loader
    ${PROJECT_SOURCE_DIR}/macro
    ${PROJECT_SOURCE_DIR}/utils
    ${PROJECT_SOURCE_DIR}/ui
    ${PROJECT_SOURCE_DIR}/vm
)

# --- SDL2 ---
set(SDL2_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/ui/include/SDL2)
set(SDL2_LIBRARY ${PROJECT_SOURCE_DIR}/ui/lib/SDL2.lib)   # import lib para a DLL
set(SDL2MAIN_LIBRARY ${PROJECT_SOURCE_DIR}/ui/lib/SDL2main.lib)
include_directories(${SDL2_INCLUDE_DIR})

# --- SOURCES ---
file(GLOB_RECURSE SOURCES
    ${PROJECT_SOURCE_DIR}/ui/imgui/*.cpp
    ${PROJECT_SOURCE_DIR}/ui/*.cpp
    ${PROJECT_SOURCE_DIR}/assembler/*.cpp
    ${PROJECT_SOURCE_DIR}/linker/*.cpp
    ${PROJECT_SOURCE_DIR}/loader/*.cpp
    ${PROJECT_SOURCE_DIR}/macro/*.cpp
    ${PROJECT_SOURCE_DIR}/utils/*.cpp
    ${PROJECT_SOURCE_DIR}/vm/*.cpp
    ${PROJECT_SOURCE_DIR}/main.cpp
)

list(FILTER SOURCES EXCLUDE REGEX "CMakeFiles")

# --- EXECUTABLE ---
add_executable(BabbageVM ${SOURCES})

# --- LINKING ---
target_link_libraries(BabbageVM
    ${SDL2MAIN_LIBRARY}
    ${SDL2_LIBRARY}
)

# --- COPY DLL AND ASSETS ---
add_custom_command(TARGET BabbageVM POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${PROJECT_SOURCE_DIR}/ui/lib/SDL2.dll
    $<TARGET_FILE_DIR:BabbageVM>
)

# --- COPY ASSETS ---
add_custom_command(TARGET BabbageVM POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/ui/assets $<TARGET_FILE_DIR:BabbageVM>/ui/assets
)