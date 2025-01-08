cmake_minimum_required(VERSION 3.14)  # Minimum version of CMake required

# Project name, version, and language
project(bril-superopt
        VERSION 1.0.0
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Optionally, set compiler-specific flags (example for GCC/Clang)
# You can remove or modify these lines if you prefer not to set extra flags
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -fno-omit-frame-pointer")

# Add an executable
# The list of source files can be expanded as your project grows
file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/*.cpp")
add_executable(${PROJECT_NAME} ${SRC_FILES})

# Add include directories for headers
target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)

# Optionally, you can link libraries if needed
# target_link_libraries(${PROJECT_NAME} PRIVATE some_library)

# (Optional) Installation instructions, if you plan to install your project
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(FILES include/MyClass.hpp DESTINATION include)

# (Optional) Add tests by including a CTest-based script or separate tests
# enable_testing()
# add_subdirectory(tests)
