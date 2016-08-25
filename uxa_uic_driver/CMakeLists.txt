cmake_minimum_required(VERSION 2.8.3)
project(uxa_uic_driver)

find_package(catkin REQUIRED COMPONENTS
  uxa_serial_msgs
  uxa_uic_msgs
  roscpp
)


catkin_package(
  INCLUDE_DIRS include
  LIBRARIES uxa_uic_driver
  CATKIN_DEPENDS roscpp
  DEPENDS system_lib
)

include_directories(include/
)


add_executable(uxa_uic_driver src/uxa_uic_driver.cpp src/uic_packet.cpp)
 target_link_libraries(uxa_uic_driver
   ${catkin_LIBRARIES}
 )

add_dependencies(uxa_uic_driver uxa_serial_msgs_generate_messages_cpp)
add_dependencies(uxa_uic_driver uxa_uic_msgs_generate_messages_cpp)
