cmake_minimum_required(VERSION 2.8.3)
project(uxa_sam_driver)

find_package(catkin REQUIRED COMPONENTS
  uxa_serial_msgs
  uxa_sam_msgs
  roscpp
)


catkin_package(
  INCLUDE_DIRS include
  LIBRARIES uxa_sam_driver
  CATKIN_DEPENDS roscpp
  DEPENDS system_lib
)

include_directories(include
)


add_executable(uxa_sam_driver src/uxa_sam_driver.cpp src/sam_packet.cpp)
 target_link_libraries(uxa_sam_driver
   ${catkin_LIBRARIES}
 )

add_dependencies(uxa_sam_driver uxa_serial_msgs_generate_messages_cpp)
add_dependencies(uxa_sam_driver uxa_sam_msgs_generate_messages_cpp)
