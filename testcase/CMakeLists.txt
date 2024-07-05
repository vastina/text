set(SDL2_ROOT "${env}/SDL2/2.30.0")
find_package(SDL2 REQUIRED)
set(SDL2_image_ROOT "${env}/SDL2_image/2.8.2")
find_package(SDL2_image REQUIRED)
add_executable(main main.cpp)
target_include_directories(main PRIVATE 
  ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} 
  ${freetype_SOURCE_DIR}/include ${freetype_SOURCE_DIR}/include/freetype 
  ${SDL2_INCLUDE_DIRS} ${SDL2_IMAGE_INCLUDE_DIRS}
)
target_link_libraries(main PRIVATE 
  ${project_name} 
  ${SDL2_LIBRARIES} SDL2_image::SDL2_image 
  freetype
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET main PROPERTY CXX_STANDARD 20)
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})