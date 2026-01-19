cmake_minimum_required(VERSION 3.16)
project(MyMathLang)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

set(SOURCES
    src/main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/typechecker.cpp
    src/codegen.cpp
)

set(HEADERS
    include/lexer.h
    include/parser.h
    include/typechecker.h
    include/codegen.h
)

add_executable(mmlc ${SOURCES} ${HEADERS})
