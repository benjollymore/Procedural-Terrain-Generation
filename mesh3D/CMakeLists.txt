get_filename_component(EXERCISENAME ${CMAKE_CURRENT_LIST_DIR} NAME)
file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE SHADERS "*.glsl")

add_executable(${EXERCISENAME} ${SOURCES} ${HEADERS} ${SHADERS})
target_link_libraries(${EXERCISENAME} ${COMMON_LIBS})
#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_LIST_DIR})

#--- See meshes in project
file(GLOB_RECURSE MESHES "*.obj")
add_custom_target(dummy_meshes SOURCES ${MESHES})
