cmake_minimum_required(VERSION 3.10)
project(interpreter)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Опции для отладки
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wpedantic -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

# Все исходные файлы
set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/token.cpp
    src/parser.cpp
    src/ast.cpp
    src/environment.cpp
    src/interpreter.cpp
)

# Все заголовочные файлы
set(HEADERS
    src/lexer.h
    src/token.h
    src/parser.h
    src/ast.h
    src/environment.h
    src/interpreter.h
)

# Создаем исполняемый файл
add_executable(interpreter ${SOURCES} ${HEADERS})

# Устанавливаем пути для include файлов
target_include_directories(interpreter PRIVATE src)

# Для Windows добавляем необходимые флаги
if(WIN32)
    target_compile_definitions(interpreter PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# Опционально: установка для системы
install(TARGETS interpreter DESTINATION bin)

# Опционально: тестовые цели
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/test_programs)
    file(GLOB TEST_PROGRAMS "test_programs/*.txt")
    foreach(test_file ${TEST_PROGRAMS})
        get_filename_component(test_name ${test_file} NAME_WE)
        add_test(NAME test_${test_name}
                 COMMAND interpreter ${test_file}
                 WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    endforeach()
endif()

# Информация о проекте
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Source files: ${SOURCES}")
message(STATUS "Header files: ${HEADERS}")
