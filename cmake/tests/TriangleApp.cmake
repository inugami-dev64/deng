# DENG: dynamic engine - powerful 3D game engine
# licence: Apache, see LICENCE file
# file: TriangleApp.cmake - TriangleApp test application CMake configuration file
# author: Karl-Mihkel Ott

# OpenGLTriangle application
set(OGL_TRIANGLE_TARGET OpenGLTriangle)
set(OGL_TRIANGLE_SOURCES tests/OpenGLTriangle.cpp)

add_executable(${OGL_TRIANGLE_TARGET} ${OGL_TRIANGLE_SOURCES})
add_dependencies(${OGL_TRIANGLE_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${OGL_TRIANGLE_TARGET} ${DENG_COMPLETE_TARGET})

# VulkanTriangle application
set(VK_TRIANGLE_TARGET VulkanTriangle)
set(VK_TRIANGLE_SOURCES tests/VulkanTriangle.cpp)

add_executable(${VK_TRIANGLE_TARGET} ${VK_TRIANGLE_SOURCES})
add_dependencies(${VK_TRIANGLE_TARGET} ${DENG_COMPLETE_TARGET})
target_link_libraries(${VK_TRIANGLE_TARGET} ${DENG_COMPLETE_TARGET})

# Custom commands regarding runtime data
add_custom_command(TARGET ${OGL_TRIANGLE_TARGET}
    POST_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/TriangleAppData
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/tests/TriangleAppData/ ${CMAKE_CURRENT_BINARY_DIR}/TriangleAppData
)
