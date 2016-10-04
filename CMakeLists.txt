cmake_minimum_required(VERSION 2.8)

project(2Dgame)

set(CMAKE_CXX_FLAGS "-g -Wall")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(GLUT)
find_package(OpenGL)
include_directories( ${OPENGL_INCLUDE_DIRS}  ${GLUT_INCLUDE_DIRS} )
set(CORELIBS ${GLUT_LIBRARY} ${OPENGL_LIBRARY})


add_executable(2Dgame main.cpp RGBpixmap.cpp)
target_link_libraries(2Dgame ${CORELIBS})

# create the launcher for vs 2010
#create_target_launcher(2Dgame ARGS "${COMMAND_LINE_ARGS}" RUNTIME_LIBRARY_DIRS ${RUNTIME_LIBRARY_DIRS} WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin/" ENVIRONMENT ${ENVIRONMENT_VARS})
#if(MSVC_IDE)
#    create_target_launcher(2Dgame ARGS "${COMMAND_LINE_ARGS}" WORKING_DIRECTORY )
#endif()
