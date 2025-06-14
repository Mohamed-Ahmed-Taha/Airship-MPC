cmake_minimum_required(VERSION 2.8.3)
add_compile_options(-std=c++11)
project(nmpc_blimp_formation_planner_msgs)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  message_generation
)

add_message_files(
  FILES
  OptimizationResult.msg
  OptimizationParameters.msg
)

generate_messages(
)

catkin_package(
   CATKIN_DEPENDS message_runtime
)