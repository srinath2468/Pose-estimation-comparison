#!/usr/bin/env python  

'''
[ INFO] [1552064121.022665536]: 3) TOPIC STREAM:
[ INFO] [1552064121.024991520]: hand_camera: 
translation: 
  x: -0.976348
  y: -0.368274
  z: 0.492565
rotation: 
  x: -0.729964
  y: 0.330928
  z: -0.265701
  w: 0.535764

'''

import rospy
import tf2_ros
import tf2_msgs.msg
import geometry_msgs.msg
from tf import transformations 
import tf
import numpy as np
#import tf.transformations as transformations
from tf.transformations import *
class FixedTFBroadcaster:


    def __init__(self):
        self.pub_tf_cam_map = rospy.Publisher("/tf", tf2_msgs.msg.TFMessage, queue_size=3)
        self.pub_tf_cam_world = rospy.Publisher("/tf", tf2_msgs.msg.TFMessage, queue_size=3)
        self.pub_tf_map_world = rospy.Publisher("/tf", tf2_msgs.msg.TFMessage, queue_size=3)

        while not rospy.is_shutdown():
            # Run this loop at about 10Hz
            rospy.sleep(0.1)

            t = geometry_msgs.msg.TransformStamped()
            t.header.frame_id = "map"
            t.header.stamp = rospy.Time.now()
            t.child_frame_id = "camera_frame"
            t.transform.translation.x = 0.0
            t.transform.translation.y = 0.0
            t.transform.translation.z = 0.0

            t.transform.rotation.w = 0.5
            t.transform.rotation.x = -0.5
            t.transform.rotation.y = 0.5
            t.transform.rotation.z = -0.5

            tfm = tf2_msgs.msg.TFMessage([t])
            self.pub_tf_cam_map.publish(tfm)
                        
            t_cam_world = geometry_msgs.msg.TransformStamped()
            t_cam_world.header.frame_id = "world"
            t_cam_world.header.stamp = rospy.Time.now()
            t_cam_world.child_frame_id = "camera_frame"
            t_cam_world.transform.translation.x = -0.976348
            t_cam_world.transform.translation.y =  -0.368274
            t_cam_world.transform.translation.z = 0.492565
            t_cam_world.transform.rotation.x =   -0.729964
            t_cam_world.transform.rotation.y =   0.330928
            t_cam_world.transform.rotation.z = -0.265701
            t_cam_world.transform.rotation.w =   0.535764

            

            tfm = tf2_msgs.msg.TFMessage([t_cam_world])
            self.pub_tf_cam_world.publish(tfm)
            
            cam_map_quat = (t.transform.rotation.x,t.transform.rotation.y,t.transform.rotation.z,t.transform.rotation.w)
            inv_quaternion = transformations.quaternion_inverse(cam_map_quat)
            cam_world_quat = (t_cam_world.transform.rotation.x,t_cam_world.transform.rotation.y,t_cam_world.transform.rotation.z,t_cam_world.transform.rotation.w)
            
            map_quat = quaternion_multiply( cam_world_quat, inv_quaternion)
            
	    t_map_world = geometry_msgs.msg.TransformStamped()
            t_map_world.header.frame_id = "world"
            t_map_world.header.stamp = rospy.Time.now()
            t_map_world.child_frame_id = "map"
            t_map_world.transform.translation.x =  -0.976348
            t_map_world.transform.translation.y = -0.368274
            t_map_world.transform.translation.z =0.492565

            t_map_world.transform.rotation.w = map_quat[3]
            t_map_world.transform.rotation.x = map_quat[0]
            t_map_world.transform.rotation.y = map_quat[1]
            t_map_world.transform.rotation.z = map_quat[2]

            tfm = tf2_msgs.msg.TFMessage([t_map_world])
            self.pub_tf_map_world.publish(tfm)
	    
	    
            
            
         

if __name__ == '__main__':
    rospy.init_node('broadcast_frame')
    tfb = FixedTFBroadcaster()

    rospy.spin()