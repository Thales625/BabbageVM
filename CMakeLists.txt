cmake_minimum_required(VERSION 3.16)
project(babbage LANGUAGES CXX)

# Define a versão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adiciona os arquivos fonte
add_executable(babbage
    assembler/assembler.cpp
    linker/linker.cpp
    linker/linkerutils.cpp
    loader/loader.cpp
    macro/macro.cpp
    vm/bitmap.cpp
    vm/cpu.cpp
    utils/utils.cpp
    main.cpp
)

# Inclui os diretórios com headers
target_include_directories(babbage PRIVATE
    assembler
    linker
    loader
    macro
    vm
    utils
)