cmake_minimum_required(VERSION 3.13.4)

project(best_program)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS -m64)

set(CMAKE_BUILD_TYPE Debug)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

add_compile_options(-g)

#project

file(GLOB vulkan_test_SRC *.cpp vk/*.cpp)
list(FILTER vulkan_test_SRC EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/\#*.cpp)
list(FILTER vulkan_test_SRC EXCLUDE REGEX ${CMAKE_SOURCE_DIR}/*/\#*.cpp)

add_executable(${PROJECT_NAME} ${vulkan_test_SRC}) 

# libs

link_directories(/usr/lib)

target_link_libraries(${PROJECT_NAME} PRIVATE libvulkan.so libglfw.so)

add_subdirectory(../libs/spdlog ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog)

