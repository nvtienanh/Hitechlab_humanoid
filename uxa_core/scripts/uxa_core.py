#!/usr/bin/env python
import rospy
import roslaunch

package = 'uxa_balltracking'
executable = 'uxa_balltracking'
node = roslaunch.core.Node(package, executable)

launch = roslaunch.scriptapi.ROSLaunch()
launch.start()

process = launch.launch(node)
print process.is_alive()
count = 0
while count <=1000000000000:
    count=count+1
process.stop()
