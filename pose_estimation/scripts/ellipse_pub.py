#!/usr/bin/env python  



import rospy
import tf2_ros
import tf2_msgs.msg
import geometry_msgs.msg
from tf import transformations 
import tf
import numpy as np
#import tf.transformations as transformations
from tf.transformations import *#!/usr/bin/env python  
from marker_msgs.msg import MarkerDetection

from geometry_msgs.msg import TransformStamped, Transform, Vector3, Quaternion, PoseStamped

import rospy
import tf2_ros
import tf2_msgs.msg
import geometry_msgs.msg
from tf import transformations 
import tf
import numpy as np
#import tf.transformations as transformations
from tf.transformations import *




def call(lol):
  global arucopub
  print("hey")
  a = lol.markers
  if len(a) == 1:
    print(a[0].pose)
    def republish_pose(a):
      
      new_pose = PoseStamped()
      new_pose.header.frame_id ="zed_left_camera_optical_frame"
      new_pose.pose.position = a[0].pose.position
      new_pose.pose.orientation = a[0].pose.orientation

      
      arucopub.publish(new_pose)
      
    republish_pose(a)














if __name__ == '__main__':
    rospy.init_node('aruco_pub')
    global arucopub
    aruco = rospy.get_param('~aruco_topic_name')
    arucopub = rospy.Publisher('testing',PoseStamped,queue_size=10)
    rospy.Subscriber("marker",MarkerDetection,call)
    
    
    rospy.spin()