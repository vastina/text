cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()
cmake_policy(SET CMP0074 NEW) # for xxx_root
set(env "C:/ProgramData/scoop/apps") # packages location
include(FetchContent) # FetchContent_Declare FetchContent_MakeAvailable
include_directories("D:\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.40.33807\\include") # clangd cat't find it automatically

project(text-render)

# Zlib
FetchContent_Declare(
  zlib
  URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip
)
# FreeType
FetchContent_MakeAvailable(zlib)
FetchContent_Declare(
  freetype
  SOURCE_DIR "${env}/freetype/2.13.2"
  # clone once to local
  # GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
)
FetchContent_MakeAvailable(freetype)
# sdl2
set(SDL2_ROOT "${env}/SDL2/2.30.0")
find_package(SDL2 REQUIRED)
# sdl2_image
set(SDL2_image_ROOT "${env}/SDL2_image/2.8.2")
find_package(SDL2_image REQUIRED)


set(LIBBASE base)
set(BASE_INCLUDE "${CMAKE_SOURCE_DIR}/${LIBBASE}/include")
file(GLOB SOURCES_BASE "${CMAKE_SOURCE_DIR}/${LIBBASE}/*.cpp")
add_library(${LIBBASE} STATIC ${SOURCES_BASE})
target_include_directories(${LIBBASE} PRIVATE ${BASE_INCLUDE} ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
target_link_libraries(${LIBBASE} zlibstatic)

set(MAINPROJECT render)
set(RENDER_INCLUDE "${CMAKE_SOURCE_DIR}/${MAINPROJECT}/include")
file(GLOB SOURCES_RENDER "${CMAKE_SOURCE_DIR}/${MAINPROJECT}/*.cpp")
add_executable(render ${SOURCES_RENDER})
target_include_directories(${MAINPROJECT} PRIVATE
  ${BASE_INCLUDE} ${RENDER_INCLUDE}
  ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} 
  ${freetype_SOURCE_DIR}/include ${freetype_SOURCE_DIR}/include/freetype 
  ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS}
)
target_link_libraries(${MAINPROJECT} PRIVATE 
  ${LIBBASE}
  ${SDL2_LIBRARIES} SDL2_image::SDL2_image 
  freetype
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${LIBBASE} PROPERTY CXX_STANDARD 20)
  set_property(TARGET ${MAINPROJECT} PROPERTY CXX_STANDARD 20)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})