#! /usr/bin/env python

import rospy
from geometry_msgs.msg import PoseStamped
import math
import actionlib

import visp_hand2eye_calibration.msg

camera_object_pose = PoseStamped()
ee_pose = PoseStamped()

def camera_callback(msg):
  global camera_object_pose
  camera_object_pose = msg
  

def ee_callback(msg):
  global ee_pose
  ee_pose = msg
  

class getposesAction(object):
  # create messages that are used to publish feedback/result
  _feedback = visp_hand2eye_calibration.msg.getposesFeedback()
  _result   = visp_hand2eye_calibration.msg.getposesResult()
  
  
  def __init__(self, name):
    self._action_name = name
    self._as = actionlib.SimpleActionServer(self._action_name, visp_hand2eye_calibration.msg.getposesAction, execute_cb=self.execute_cb)
    self._as.start()
    
  def execute_cb(self, goal):
    # helper variables
    r = rospy.Rate(30)
    success = True
    
    if self._as.is_preempt_requested():
      rospy.loginfo('%s: Preempted' % self._action_name)
      self._as.set_preempted()
      success = False
      
    r.sleep()
      
    if success:
      global camera_object_pose
      global ee_pose
      self._result.camera_object_pose = camera_object_pose
      self._result.ee_pose = ee_pose
      print(ee_pose)
      rospy.loginfo('%s: Succeeded' % self._action_name)
      self._as.set_succeeded(self._result)


if __name__ == '__main__':
  rospy.init_node('get_points_server')
  getposesAction(rospy.get_name())
  rospy.Subscriber("/checkerpose", PoseStamped, camera_callback)
  rospy.Subscriber("/tcp_pose", PoseStamped, ee_callback)
  
  rospy.spin()


