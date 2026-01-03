cmake_minimum_required(VERSION 3.10)
project(Lab2)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
    set(THREADS_LIBS kernel32 user32 gdi32 winspool shell32 ole32 oleaut32 uuid comdlg32 advapi32)
endif()

add_executable(Lab2 
    lab2.cpp
    core/array.cpp
    threads/thread.cpp
 "tests/test_lab2.cpp")

if(WIN32)
    target_link_libraries(Lab2 ${THREADS_LIBS})
endif()

add_executable(Lab2_Tests 
    tests/test_lab2.cpp 
    core/array.cpp
 "tests/test_lab2.cpp")

if(WIN32)
    target_link_libraries(Lab2_Tests ${THREADS_LIBS})
endif()