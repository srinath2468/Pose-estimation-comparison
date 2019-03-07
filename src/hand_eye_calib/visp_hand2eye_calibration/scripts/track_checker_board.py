#! /usr/bin/env python
from __future__ import print_function

import roslib
roslib.load_manifest('yiwu_vision')
import rospy
from cv_bridge import CvBridge, CvBridgeError
import actionlib
import message_filters
from sensor_msgs.msg import Image, PointCloud2, CameraInfo
import sensor_msgs.point_cloud2 as pcl2
import pcl_ros as pcl_ros
import cv2
import numpy as np
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
from geometry_msgs.msg import TransformStamped, Transform, Vector3, Quaternion
from tf2_sensor_msgs.tf2_sensor_msgs import do_transform_cloud #install tf2_sensor_msgs - "sudo apt-get install ros-kinetic-tf2-sensor-msgs"
import tf.transformations

lock = bool(0)
#call back to receive the camera data  

def callback(image, pcl, camInfo_msg):
    cv_image = CvBridge().imgmsg_to_cv2(image,"bgr8")
    #cv2.imshow("camera input", cv_image)
    
    img_row = cv_image.shape[0]
    img_col = cv_image.shape[1]
    print(cv_image.shape)
    
    pcl_array = ros_np.point_cloud2.pointcloud2_to_array(pcl)
    
    gray=cv2.cvtColor(cv_image, cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, (7,6))
    # termination criteria
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)

    # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
    objp = np.zeros((6*7,3), np.float32)
    objp[:,:2] = np.mgrid[0:7,0:6].T.reshape(-1,2)

    # Arrays to store object points and image points from all the images.
    objpoints = [] # 3d point in real world space
    imgpoints = [] # 2d points in image plane.

    if ret == True:
      objpoints.append(objp)
      corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
      imgpoints.append(corners2)
      
      f = (corners.squeeze())
      f = f.astype(int)
      cv2.cornerSubPix(gray,corners,(5,5),(-1,-1),criteria)
      tempimg = gray.copy()
      pcl_array = ros_np.point_cloud2.pointcloud2_to_array(pcl)

      # Draw and display the corners
      img = cv2.drawChessboardCorners(cv_image, (7,6), corners2,ret)
      cv2.imshow('img',cv_image)
      cv2.waitKey(3)

if __name__ == '__main__':
    rospy.init_node('track_checker_board')
    
    #subscribe to camera topics
    if rospy.has_param('/track_checker_board/img_topic_name') & rospy.has_param('/track_checker_board/pcl_topic_name'):
      
      img_topic_name = rospy.get_param('~img_topic_name')
      pcl_topic_name = rospy.get_param('~pcl_topic_name')
      cam_info_topic_name = rospy.get_param('~cam_info_topic_name')
      
      image_sub = message_filters.Subscriber(img_topic_name, Image)
      pcl_sub = message_filters.Subscriber(pcl_topic_name, PointCloud2)
      camera_info_sub = message_filters.Subscriber(cam_info_topic_name,CameraInfo)
      
      ts = message_filters.ApproximateTimeSynchronizer([image_sub, pcl_sub, camera_info_sub],  10, 0.1, allow_headerless=True)
      ts.registerCallback(callback)
      
    else:
      rospy.logerr("One or all camera topic names are not set. Please make sure the name parameters are set. Either using ROS param or in launch file")
    
        
    rospy.spin()
