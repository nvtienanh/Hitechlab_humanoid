cmake_minimum_required(VERSION 2.8.3)
project(uxa_uic_msgs)

find_package(catkin REQUIRED COMPONENTS
  std_msgs
  message_runtime
  message_generation
)

 add_message_files(
   DIRECTORY msg
   FILES
   remocon.msg
   motion.msg
)

 generate_messages(
   DEPENDENCIES
   std_msgs
)

catkin_package(
   CATKIN_DEPENDS
   std_msgs
   message_runtime
   message_generation
)

install(DIRECTORY include/${PROJECT_NAME}/
	DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
	FILES_MATCHING PATTERN "*.h"
)



