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

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address,undefined -fno-omit-frame-pointer -g -O0")

file(GLOB_RECURSE SRC_FILES "${PROJECT_SOURCE_DIR}/src/IR/*.cpp")

# Fetch the nlohmann/json library
include(FetchContent)
FetchContent_Declare(json URL 
    https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz
        DOWNLOAD_EXTRACT_TIMESTAMP true)
FetchContent_MakeAvailable(json)

# Fetch the cpptrace library
include(FetchContent)
FetchContent_Declare(
  cpptrace
  GIT_REPOSITORY https://github.com/jeremy-rifkin/cpptrace.git
  GIT_TAG        v0.7.5
)
FetchContent_MakeAvailable(cpptrace)

# Add an executable for json2bril
add_executable(json2bril ${SRC_FILES} "${PROJECT_SOURCE_DIR}/src/json2bril.cpp")
target_include_directories(json2bril PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(json2bril PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(json2bril PRIVATE cpptrace::cpptrace)

# Add an executable for brili
add_executable(brili ${SRC_FILES} "${PROJECT_SOURCE_DIR}/src/brili.cpp")
target_include_directories(brili PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(brili PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(brili PRIVATE cpptrace::cpptrace)


# (Optional) Installation instructions, if you plan to install your project
# install(TARGETS ${PROJECT_NAME} DESTINATION bin)
# install(FILES include/MyClass.hpp DESTINATION include)

# (Optional) Add tests by including a CTest-based script or separate tests
# enable_testing()
# add_subdirectory(tests)
