#!/usr/bin/env python2
import rospy
import numpy as np
from nav_msgs.msg import OccupancyGrid
# the following line depends upon the
# type of message you are trying to publish
from std_msgs.msg import String
from std_msgs.msg import Float32MultiArray

"""
Float32MultiArray:
std_msgs/MultiArrayLayout layout
	std_msgs/MultiArrayDimension[] dim
		string label
		uint32 size
		uint32 stride
	uint32 data_offset


float32[] data

"""

#Callback function
def callback1(msg):
    rospy.loginfo(msg.data)

def callback2(msg):
    rospy.loginfo(msg.data)


if __name__ == "__main__":
	#Initialize ROS node
	# rospy.init_node("Astar_globel_path_planning",anonymous=True)
	# initialize the publishing node
	rospy.init_node('find_transformation', anonymous=True)
	
	pub = rospy.Publisher('/transformation_matrix',Float32MultiArray, queue_size=10)
	
    #Accept data
	sub1 = rospy.Subscriber("/robot1/map",OccupancyGrid, callback1,queue_size=10)
	sub2 = rospy.Subscriber("/robot2/map",OccupancyGrid, callback2,queue_size=10)


	# define how many times per second
	# will the data be published
	# let's say 10 times/second or 10Hz
	rate = rospy.Rate(10)

	x = 1
	y = 1
	theta = 0
	# to keep publishing as long as the core is running
	while not rospy.is_shutdown():

		data = Float32MultiArray()
		# data.layout.dim[0].label = "label"
		# data.layout.dim[0].size = 1
		# data.layout.dim[0].stride = 1
		# data.layout.data_offset = 0

		data.data = np.array([x, y, theta])

		# you could simultaneously display the data
		# on the terminal and to the log file
		rospy.loginfo(data)
		
		# publish the data to the topic using publish()
		pub.publish(data)
		
		# keep a buffer based on the rate defined earlier
		# Same function as spinOnce
		rate.sleep()

