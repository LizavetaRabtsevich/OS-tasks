# CMakeList.txt: проект CMake для lab2_cmake; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (lab2_cmake "lab2_cmake.cpp" "lab2_cmake.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET lab2_cmake PROPERTY CXX_STANDARD 20)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
