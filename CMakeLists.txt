# CMakeList.txt : CMake project for img, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories("D:\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.40.33807\\include")
include_directories(${CMAKE_SOURCE_DIR})
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
cmake_policy(SET CMP0074 NEW)
include(FetchContent)

set(project_name img)
project (${project_name})

# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(env "C:/ProgramData/scoop/apps")


# 下载和添加 Zlib
FetchContent_Declare(
  zlib
  URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip
)
FetchContent_MakeAvailable(zlib)
FetchContent_Declare(
  freetype
  SOURCE_DIR "${env}/freetype/2.13.2"
  # clone once to local
  # GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
)
FetchContent_MakeAvailable(freetype)

include_directories("${CMAKE_SOURCE_DIR}/code/include")
file(GLOB SOURCES "${CMAKE_SOURCE_DIR}/code/*.cpp")
add_library(${project_name} STATIC ${SOURCES})
target_include_directories(${project_name} PRIVATE ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
target_link_libraries(${project_name} zlibstatic)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${project_name} PROPERTY CXX_STANDARD 20)
endif()

add_subdirectory(testcase)
# set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})