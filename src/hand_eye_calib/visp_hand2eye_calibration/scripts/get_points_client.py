#! /usr/bin/env python
from __future__ import print_function
import rospy
import actionlib
import visp_hand2eye_calibration.msg
import cv2 as cv
import yaml
import rospkg
from geometry_msgs.msg import PoseStamped
import sys

def write_data():
    rospack = rospkg.RosPack()
    file_path = rospack.get_path('visp_hand2eye_calibration')+"/data/calib_points.yaml"
    result = getpoints_client()
    with open(file_path, 'r') as stream:
      data = yaml.load(stream)
      index = len(data['camera_object_points'])
      data['camera_object_points'].insert(index,result.camera_object_pose.pose.position.x)
      data['camera_object_points'].insert(index+1,result.camera_object_pose.pose.position.y)
      data['camera_object_points'].insert(index+2,result.camera_object_pose.pose.position.z)
      data['camera_object_points'].insert(index+3,result.camera_object_pose.pose.orientation.x)
      data['camera_object_points'].insert(index+4,result.camera_object_pose.pose.orientation.y)
      data['camera_object_points'].insert(index+5,result.camera_object_pose.pose.orientation.z)
      data['camera_object_points'].insert(index+6,result.camera_object_pose.pose.orientation.w)
      index = len(data['world_effector_points'])
      data['world_effector_points'].insert(index,result.ee_pose.pose.position.x)
      data['world_effector_points'].insert(index+1,result.ee_pose.pose.position.y)
      data['world_effector_points'].insert(index+2,result.ee_pose.pose.position.z)
      data['world_effector_points'].insert(index+3,result.ee_pose.pose.orientation.x)
      data['world_effector_points'].insert(index+4,result.ee_pose.pose.orientation.y)
      data['world_effector_points'].insert(index+5,result.ee_pose.pose.orientation.z)
      data['world_effector_points'].insert(index+6,result.ee_pose.pose.orientation.w)      
    print(data)
    with open(file_path, 'w') as outfile:
      yaml.dump(data, outfile, default_flow_style=False)
    

def getpoints_client():

    client = actionlib.SimpleActionClient('get_points_server', visp_hand2eye_calibration.msg.getposesAction)

    client.wait_for_server()

    goal = visp_hand2eye_calibration.msg.getposesGoal(target_id=20)

    client.send_goal(goal)

    client.wait_for_result()

    return client.get_result()  

if __name__ == '__main__':
  
    rospack = rospkg.RosPack()
    file_path = rospack.get_path('visp_hand2eye_calibration')+"/data/calib_points.yaml"
    clear_data = dict(
    camera_object_points = [],
    world_effector_points = [],
    )
    with open(file_path, 'w') as outfile:
      yaml.dump(clear_data, outfile, default_flow_style=False)
      
    try:
        rospy.init_node('getpoints_client_py')
        key = "d"
        print("================================================")
        print("Press s to save the points.\nPress esc to exit")
        print("================================================")
        count = 0
        camera_obj_poses = []
        ee_obj_poses = []
        while(key!="c"):
	  key = raw_input("press c to cancel. s to save the points. press f to finish collection")
	  print("==========Point set#", count, "==============" )
	  if key == "s":
	    write_data()
	    print(count)
	    count = count + 1	    
	  elif key == "c":
	    continue
	  else:
	    print("please enter a valid key")	  
	  
    except rospy.ROSInterruptException:
        print("program interrupted before completion", file=sys.stderr)