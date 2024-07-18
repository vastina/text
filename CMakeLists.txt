cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if(WIN32)
  if (POLICY CMP0141)
    cmake_policy(SET CMP0141 NEW)
    set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
  endif()
  cmake_policy(SET CMP0074 NEW) # for xxx_root
  set(env "C:/ProgramData/scoop/apps") # packages location
  include(FetchContent) # FetchContent_Declare FetchContent_MakeAvailable
  include_directories("D:\\Microsoft Visual Studio\\2022\\Professional\\VC\\Tools\\MSVC\\14.40.33807\\include") # clangd cat't find it automatically
endif()

project(text-render)
if(LINUX)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_BUILD_TYPE Debug)
endif()

#find_packages#########################################################################################
###############
# Zlib
if(WIN32)  
  FetchContent_Declare(
    zlib
    SOURCE_DIR "${env}/zlib/1.3.1"
    # URL https://github.com/madler/zlib/archive/refs/tags/v1.3.1.zip
  )
  FetchContent_MakeAvailable(zlib)
elseif(LINUX)
  find_package(ZLIB REQUIRED)
endif()

# FreeType
if(WIN32)  
  FetchContent_Declare(
    freetype
    SOURCE_DIR "${env}/freetype/2.13.2"
    # clone once to local
    # GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
  )
  FetchContent_MakeAvailable(freetype)
elseif(LINUX)
  find_package(Freetype REQUIRED)
endif()

# utfcpp head only
if(WIN32)  
  set(utfcpp_INCLUDE_DIR "${env}/utfcpp/source")
  # https://github.com/nemtrif/utfcpp.git
elseif(LINUX)
  set(utfcpp_INCLUDE_DIR "/usr/include/utf8cpp")
endif()

# sdl2
if(WIN32)
  set(SDL2_ROOT "${env}/SDL2/2.30.0")
endif()
find_package(SDL2 REQUIRED)

# # sdl2_image
# if(WIN32)
#   set(SDL2_image_ROOT "${env}/SDL2_image/2.8.2")
# endif()
# find_package(SDL2_image REQUIRED)

# x11
if(LINUX)
  find_package(X11 REQUIRED)
endif()
###############
#######################################################################################################

set(LIBBASE base)
set(BASE_INCLUDE "${CMAKE_SOURCE_DIR}/${LIBBASE}/include")
file(GLOB SOURCES_BASE "${CMAKE_SOURCE_DIR}/${LIBBASE}/*.cpp")
add_library(${LIBBASE} STATIC ${SOURCES_BASE})
if(WIN32)
  target_include_directories(${LIBBASE} PRIVATE ${BASE_INCLUDE} ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR})
  target_link_libraries(${LIBBASE} PRIVATE zlibstatic)
elseif(LINUX)
  target_include_directories(${LIBBASE} PRIVATE ${BASE_INCLUDE} ${ZLIB_INCLUDE_DIRS})
  target_link_libraries(${LIBBASE} PRIVATE ${ZLIB_LIBRARIES})
endif()

set(MAINPROJECT render)
set(RENDER_INCLUDE "${CMAKE_SOURCE_DIR}/${MAINPROJECT}/include")
file(GLOB SOURCES_RENDER "${CMAKE_SOURCE_DIR}/${MAINPROJECT}/*.cpp")
add_executable(render ${SOURCES_RENDER})

if(WIN32)
  target_include_directories(${MAINPROJECT} PRIVATE
    ${BASE_INCLUDE} ${RENDER_INCLUDE}
    ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} 
    ${freetype_SOURCE_DIR}/include ${freetype_SOURCE_DIR}/include/freetype 
    ${SDL2_INCLUDE_DIRS} # ${SDL2_IMAGE_INCLUDE_DIRS}
    ${utfcpp_INCLUDE_DIR}
  )
  target_link_libraries(${MAINPROJECT} PRIVATE 
    ${LIBBASE}
    ${SDL2_LIBRARIES} # SDL2_image::SDL2_image 
    freetype
  )
elseif(LINUX)
  target_include_directories(${MAINPROJECT} PRIVATE
    ${BASE_INCLUDE} ${RENDER_INCLUDE}
    ${FREETYPE_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS} # ${SDL2_IMAGE_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
    ${utfcpp_INCLUDE_DIR}
  )
  target_link_libraries(${MAINPROJECT} PRIVATE 
    ${LIBBASE}
    ${SDL2_LIBRARIES} # SDL2_image::SDL2_image 
    ${FREETYPE_LIBRARIES}
    ${X11_LIBRARIES}
  )
endif()


if(WIN32)
  if (CMAKE_VERSION VERSION_GREATER 3.12)
    set_property(TARGET ${LIBBASE} PROPERTY CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES)
    set_property(TARGET ${MAINPROJECT} PROPERTY CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES)
  endif()
endif()

if(WIN32)
  set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})
endif()