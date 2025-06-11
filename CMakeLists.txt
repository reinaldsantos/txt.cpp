cmake_minimum_required(VERSION 3.10)
project(bank_system)

# Define o padrão C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Configurações de compilação para o projeto principal
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Adiciona o diretório de include do SQLite
include_directories(${CMAKE_SOURCE_DIR}/sqlite3/include)
include_directories(${CMAKE_SOURCE_DIR}/models)
include_directories(${CMAKE_SOURCE_DIR}/database)

# Adiciona o arquivo fonte do SQLite com flags específicas
add_library(sqlite3 STATIC sqlite3/include/sqlite3.c)
if(MSVC)
    target_compile_options(sqlite3 PRIVATE /W3)
else()
    target_compile_options(sqlite3 PRIVATE -w)
endif()

# Arquivos fonte comuns
set(COMMON_SOURCES
    database/DatabaseManager.cpp
    models/Company.cpp
)

# Arquivos de cabeçalho
set(HEADERS
    database/DatabaseManager.h
    models/Company.h
)

# Cria o executável principal (bank_system)
add_executable(bank_system
    main.cpp
    ${COMMON_SOURCES}
    ${HEADERS}
    sqlite3/include/sqlite3.c
)

# Cria o executável de visualização (view_data)
add_executable(view_data
    view_data.cpp
    ${COMMON_SOURCES}
    ${HEADERS}
)

# Linka com o SQLite
target_link_libraries(bank_system sqlite3)
target_link_libraries(view_data sqlite3)

# Adiciona os diretórios de include
target_include_directories(bank_system PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sqlite3/include
    ${CMAKE_SOURCE_DIR}/database
    ${CMAKE_SOURCE_DIR}/models
)

target_include_directories(view_data PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/sqlite3/include
    ${CMAKE_SOURCE_DIR}/database
    ${CMAKE_SOURCE_DIR}/models
)

# Configura o diretório de saída para a pasta principal
set_target_properties(bank_system view_data PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Adiciona suporte ao filesystem
if(MSVC)
    target_compile_options(bank_system PRIVATE /std:c++17)
else()
    target_link_libraries(bank_system stdc++fs)
endif() 