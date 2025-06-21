cmake_minimum_required(VERSION 3.22.1)

project("ClayEngineVKMobile")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_DEBUG_POSTFIX "" CACHE STRING "No postfix for debug libraries" FORCE) # fix for add_subdirectory adding "d"

# Collect app .cpp files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/*.cpp)

# Add source code to library
add_library(${PROJECT_NAME} SHARED ${SRC_FILES})

# Specify include directories for application code
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}
)

# Set linker flags for ANativeActivity_onCreate
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")
set_property(TARGET ${PROJECT_NAME} APPEND_STRING PROPERTY LINK_FLAGS " -u ANativeActivity_onCreate")
# Use Android version of clay
set(CLAY_PLATFORM_ANDROID ON CACHE BOOL "Set Platform to Android" FORCE)

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../../../../thirdparty/ClayEngineVK
    ${CMAKE_BINARY_DIR}/thirdparty/ClayEngineVK
)

# Link against ClayEngine
target_link_libraries(
    ${PROJECT_NAME} PRIVATE
    ClayEngineVK
)