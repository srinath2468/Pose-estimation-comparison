#! /usr/bin/env python
from __future__ import print_function

import roslib

import rospy
from cv_bridge import CvBridge, CvBridgeError
import actionlib
import message_filters
from sensor_msgs.msg import Image, PointCloud2, CameraInfo
import sensor_msgs.point_cloud2 as pcl2
import pcl_ros as pcl_ros
import cv2
import numpy as np
import math
import ros_numpy as ros_np #install ros_numpy using sudo apt-get install ros-kinetic-ros-numpy
import image_geometry, tf
from image_geometry import PinholeCameraModel
import multiprocessing #install multiprocessing libraries - "sudo pip install multiprocess"
#import numexpr as ne #install numexpr using "sudo pip install numexpr"
import math
import struct
import tf2_ros
import tf2_py as tf2
import geometry_msgs
from geometry_msgs.msg import TransformStamped, Transform, Vector3, Quaternion, PoseStamped
from tf2_sensor_msgs.tf2_sensor_msgs import do_transform_cloud #install tf2_sensor_msgs - "sudo apt-get install ros-kinetic-tf2-sensor-msgs"
import tf.transformations
from tf.transformations import *
#lock = bool(0)
#call back to receive the camera data  

def callback(image, boardpose, camInfo_msg):
  
    global posepub
    cv_image = CvBridge().imgmsg_to_cv2(image,"bgr8")
    #cv2.imshow("camera input", cv_image)
    
    img_row = cv_image.shape[0]
    img_col = cv_image.shape[1]
    #print(cv_image.shape)
    print(boardpose)
    
    quat = [boardpose.pose.orientation.x, boardpose.pose.orientation.y, boardpose.pose.orientation.z, boardpose.pose.orientation.w]
    q_rot = tf.transformations.quaternion_from_euler(-math.pi/4,0,0)
    q_rot2 = tf.transformations.quaternion_from_euler(0,0,0)
    quat = quaternion_multiply(q_rot,quat)
    quat = quaternion_multiply(q_rot2,quat)
    def republish_pose(boardpose):
      
      new_pose = PoseStamped()
      new_pose.header.frame_id ="zed_left_camera_optical_frame"
      new_pose.pose.position = boardpose.pose.position
      new_pose.pose.orientation.x = quat[0]
      new_pose.pose.orientation.y = quat[1]
      new_pose.pose.orientation.z = quat[2]
      new_pose.pose.orientation.w = quat[3]
      
      posepub.publish(new_pose)
      
    republish_pose(boardpose)
      
    #pcl_array = ros_np.point_cloud2.pointcloud2_to_array(pcl)
    
    #gray=cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    

if __name__ == '__main__':
    rospy.init_node('final')
    global posepub
    #subscribe to camera topics
    if rospy.has_param('~img_topic_name'):
      #TODO - make 1 script for noise and other stuff
      img_topic_name = rospy.get_param('~img_topic_name')
      pose_topic_name = rospy.get_param('~pose_topic_name')
      cam_info_topic_name = rospy.get_param('~cam_info_topic_name')
      
      image_sub = message_filters.Subscriber(img_topic_name, Image)
      pose_sub = message_filters.Subscriber(pose_topic_name, PoseStamped)
      camera_info_sub = message_filters.Subscriber(cam_info_topic_name,CameraInfo)
      
      posepub = rospy.Publisher('newpose', PoseStamped, queue_size=10)
      
      ts = message_filters.ApproximateTimeSynchronizer([image_sub, pose_sub, camera_info_sub],  10, 0.1, allow_headerless=True)
      ts.registerCallback(callback)
      
    else:
      rospy.logerr("One or all camera topic names are not set. Please make sure the name parameters are set. Either using ROS param or in launch file")
    
        
    rospy.spin()