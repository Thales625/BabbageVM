cmake_minimum_required(VERSION 3.10)
project(vm_project)

# Defina o padrão de compilação
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Defina a pasta onde os arquivos objeto serão armazenados
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# Adicione os arquivos fonte
set(SOURCES
    main.cpp
    cpu.cpp
    bitmap.cpp
)

# Inclua diretórios de cabeçalhos (opcional se os headers estiverem no mesmo diretório dos .cpp)
include_directories(${CMAKE_SOURCE_DIR})

# Crie o executável
add_executable(vm ${SOURCES})