cmake_minimum_required(VERSION 2.8.3)
project(uxa_core)

find_package(catkin REQUIRED COMPONENTS
  rospy
  roslaunch
)


catkin_package(
  CATKIN_DEPENDS rospy roslaunch
  DEPENDS system_lib
)

roslaunch_add_file_check(launch)

include_directories(include
)

#install(
#  PROGRAMS scripts/uxa_core.py
#  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
#)
