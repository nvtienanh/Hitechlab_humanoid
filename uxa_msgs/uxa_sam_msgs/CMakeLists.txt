cmake_minimum_required(VERSION 2.8.3)
project(uxa_sam_msgs)

find_package(catkin REQUIRED COMPONENTS
  std_msgs
  message_runtime
  message_generation
)

 add_message_files(
   DIRECTORY msg
   FILES
   position_move.msg
   std_position_move.msg
   sam_state.msg
   Num.msg
)

add_service_files(
    DIRECTORY srv
    FILES
    AddTwoInts.srv)

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



