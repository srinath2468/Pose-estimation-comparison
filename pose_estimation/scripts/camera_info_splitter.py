#! /usr/bin/env python
from __future__ import print_function

import rospy
from nerian_stereo.msg import StereoCameraInfo
from sensor_msgs.msg import CameraInfo
from sensor_msgs.msg import Image, PointCloud2, CameraInfo
import message_filters
leftPub = 0
rightPub = 0

def callback(cam,img3,pcl3):
    global leftPub;
    global rightPub;
    global pcl;
    global img;
    #rospy.loginfo(rospy.get_caller_id() + ": Camera info received")
    leftPub.publish(cam.left_info)
    rightPub.publish(cam.right_info)
    pcl.publish(pcl3)
    img.publish(img3)

  

def listener():
    global leftPub;
    global rightPub;
    global pcl;
    global img
 

    rospy.init_node('camera_info_splitter')
    cam2 = message_filters.Subscriber("/nerian_stereo/stereo_camera_info", StereoCameraInfo)
    pcl2 = message_filters.Subscriber("/nerian_stereo/point_cloud", PointCloud2)
    img2 = message_filters.Subscriber("/nerian_stereo/left_image", Image)
    #rospy.Subscriber("/nerian_stereo/disparity_map", StereoCameraInfo, callback)
    ts = message_filters.ApproximateTimeSynchronizer([cam2,img2,pcl2],  100, 10, allow_headerless=True)
    ts.registerCallback(callback)

    leftPub = rospy.Publisher('left_camera_info', CameraInfo, queue_size=10)
    rightPub = rospy.Publisher('right_camera_info', CameraInfo, queue_size=10)
    img = rospy.Publisher('camera_left_image',Image,queue_size = 10)
    pcl = rospy.Publisher('camera_point_cloud',PointCloud2,queue_size=10)
    rospy.spin()

if __name__ == '__main__':
    listener()
