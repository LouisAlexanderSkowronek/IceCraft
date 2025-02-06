cmake_minimum_required( VERSION 3.22.1 )

project( "IceCraft" )

set( GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE )
set( GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE )

set(SRCS
    "${CMAKE_SOURCE_DIR}/lib/glad/src/glad.c"
    "${CMAKE_SOURCE_DIR}/src/main.c"
    "${CMAKE_SOURCE_DIR}/src/block.c"
    "${CMAKE_SOURCE_DIR}/src/camera.c"
    "${CMAKE_SOURCE_DIR}/src/chunk.c"
    "${CMAKE_SOURCE_DIR}/src/coordinate_axes.c"
    "${CMAKE_SOURCE_DIR}/src/helper_funcs.c"
    "${CMAKE_SOURCE_DIR}/src/input_handler.c"
    "${CMAKE_SOURCE_DIR}/src/opengl_utils.c"
    "${CMAKE_SOURCE_DIR}/src/world.c"
    "${CMAKE_SOURCE_DIR}/src/gui_block_selector.c"
    "${CMAKE_SOURCE_DIR}/src/gui_crossbar.c"
    "${CMAKE_SOURCE_DIR}/src/player.c"
    "${CMAKE_SOURCE_DIR}/src/ice_craft.c"
    "${CMAKE_SOURCE_DIR}/src/texture_atlas.c"
)

add_subdirectory( lib/glfw lib/glfw/src )

add_subdirectory( "${CMAKE_SOURCE_DIR}/lib/cglm" )

add_executable( IceCraft ${SRCS} )

target_include_directories( IceCraft PUBLIC lib/glad/include lib/cglm/include lib/stb include/ )

target_link_libraries( IceCraft glfw )

target_link_libraries( IceCraft cglm )
