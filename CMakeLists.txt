cmake_minimum_required(VERSION 3.21)
project(VulkanTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)

# Include Directories for Header Files
include_directories(includes)

# Source Files
file(GLOB SOURCES "src/*.cpp")

add_executable(VulkanTest ${SOURCES})

target_link_libraries(VulkanTest PRIVATE
    glfw
    Vulkan::Vulkan
#   wayland-client
)

add_custom_target(run-all
    COMMAND VulkanTest
)

add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove VulkanTest
)

