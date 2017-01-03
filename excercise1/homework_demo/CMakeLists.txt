cmake_minimum_required(VERSION 3.6)
project(homework_demo)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
    main.cpp
    server.h
    test.cpp
    test.h
    test_server.h
    test_worker.h
    upload.cpp
    uuid.cpp
    worker.cpp
    worker.h)

add_executable(homework_demo ${SOURCE_FILES})