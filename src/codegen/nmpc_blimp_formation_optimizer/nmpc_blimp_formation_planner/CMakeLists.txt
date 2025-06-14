cmake_minimum_required(VERSION 2.8.3)
add_compile_options(-std=c++11)
project(nmpc_blimp_formation_planner)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  nmpc_blimp_formation_planner_msgs
)

catkin_package(
   CATKIN_DEPENDS nmpc_blimp_formation_planner_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include
)

set(NODE_NAME nmpc_blimp_planner)
add_executable(${NODE_NAME} src/open_optimizer.cpp)
target_link_libraries(
    ${NODE_NAME}
    ${PROJECT_SOURCE_DIR}/extern_lib/libnmpc_blimp_formation_optimizer.a)
target_link_libraries(
    ${NODE_NAME}
    m dl
    ${catkin_LIBRARIES})
add_dependencies(
    ${NODE_NAME}
    ${${PROJECT_NAME}_EXPORTED_TARGETS}
    ${catkin_EXPORTED_TARGETS})