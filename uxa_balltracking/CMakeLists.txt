cmake_minimum_required(VERSION 2.8.3)
project(uxa_balltracking)

include_directories(
  ${OpenCV_LIBS}
)

find_package(OpenCV REQUIRED
)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
  std_msgs
  uxa_uic_msgs
  uxa_sam_msgs
)

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES uxa_balltracking
  CATKIN_DEPENDS cv_bridge image_transport roscpp sensor_msgs std_msgs
  DEPENDS system_lib opencv2
)

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${OpenCV_LIBS}
)



add_executable(uxa_balltracking src/main.cpp)
 target_link_libraries(uxa_balltracking
   ${OpenCV_LIBS}
   ${catkin_LIBRARIES}
 )

add_dependencies(uxa_balltracking uxa_uic_msgs_generate_messages_cpp)
add_dependencies(uxa_balltracking uxa_sam_msgs_generate_messages_cpp)
