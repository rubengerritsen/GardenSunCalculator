cmake_minimum_required(VERSION 3.10)

project(GSC)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find dependencies
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package (Boost COMPONENTS program_options filesystem REQUIRED)
find_package(OpenMP)

# Include directory that contains header/include files
include_directories(include ${Boost_INCLUDE_DIRS})

# Read all source files from src directory
file(GLOB SOURCES "src/*.cpp")

add_compile_options(-O3 -DNDEBUG)

# Create executable
add_executable(GSC ${SOURCES})
target_link_libraries (GSC PUBLIC Eigen3::Eigen ${Boost_LIBRARIES} OpenMP::OpenMP_CXX)


