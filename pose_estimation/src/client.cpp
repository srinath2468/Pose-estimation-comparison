/****************************************************************************
 *
 * $Id: file.h 3496 2011-11-22 15:14:32Z fnovotny $
 *
 * This file is part of the ViSP software.
 * Copyright (C) 2005 - 2012 by INRIA. All rights reserved.
 *
 * This software is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * ("GPL") version 2 as published by the Free Software Foundation.
 * See the file LICENSE.txt at the root directory of this source
 * distribution for additional information about the GNU GPL.
 *
 * For using ViSP with software that can not be combined with the GNU
 * GPL, please contact INRIA about acquiring a ViSP Professional
 * Edition License.
 *
 * See http://www.irisa.fr/lagadic/visp/visp.html for more information.
 *
 * This software was developed at:
 * INRIA Rennes - Bretagne Atlantique
 * Campus Universitaire de Beaulieu
 * 35042 Rennes Cedex
 * France
 * http://www.irisa.fr/lagadic
 *
 * If you have questions regarding the use of this file, please contact
 * INRIA at visp@inria.fr
 *
 * This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
 * WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Contact visp@irisa.fr if any conditions of this licensing are
 * not clear to you.
 *
 * Description:
 * Client node
 *
 * Authors:
 * Filip Novotny
 *
 *
 *****************************************************************************/

/*!
  \file client.cpp
  \brief Client node calling a quick compute service, a compute service and 2 publishing to world_effector_topic and camera_object_topic.
*/

#include "client.h"
#include <geometry_msgs/Transform.h>
#include "visp_hand2eye_calibration/TransformArray.h"
#include <visp_bridge/3dpose.h>
#include "names.h"

#include <visp/vpCalibration.h>
#include <visp/vpExponentialMap.h>

#include <iostream>
#include <ros/package.h>
#include <ros/ros.h>
#include <string.h>
#include "yaml-cpp/yaml.h"

using namespace std;

string package_path = ros::package::getPath("visp_hand2eye_calibration");



class collect_points{
public:
  vector<geometry_msgs::Transform> world_effector_points;
  vector<geometry_msgs::Transform> camera_object_points;
  
  collect_points(string file_path);
};

collect_points::collect_points(string file_path)
{
  YAML::Node cmd = YAML::LoadFile(file_path);
  
  collect_points::world_effector_points.clear();
  collect_points::camera_object_points.clear();
  
  int no_of_points = cmd["world_effector_points"].size();
  
  cout<<"no of calibration points received: "<<(no_of_points/7)<<endl;
  
  if(no_of_points<=0){
    cout<<"error in the calib points file. please check"<<endl;
    return;
  }
  else{
    for(int i=0;i<(no_of_points/7);i++){
      geometry_msgs::Transform load_transform;
      
      load_transform.translation.x = cmd["world_effector_points"][(i*7)+0].as<double>();
      load_transform.translation.y = cmd["world_effector_points"][(i*7)+1].as<double>();
      load_transform.translation.z = cmd["world_effector_points"][(i*7)+2].as<double>();
      load_transform.rotation.x = cmd["world_effector_points"][(i*7)+3].as<double>();
      load_transform.rotation.y = cmd["world_effector_points"][(i*7)+4].as<double>();
      load_transform.rotation.z = cmd["world_effector_points"][(i*7)+5].as<double>();
      load_transform.rotation.w = cmd["world_effector_points"][(i*7)+6].as<double>();
      
      collect_points::world_effector_points.push_back(load_transform);
      
      load_transform.translation.x = cmd["camera_object_points"][(i*7)+0].as<double>();
      load_transform.translation.y = cmd["camera_object_points"][(i*7)+1].as<double>();
      load_transform.translation.z = cmd["camera_object_points"][(i*7)+2].as<double>();
      load_transform.rotation.x = cmd["camera_object_points"][(i*7)+3].as<double>();
      load_transform.rotation.y = cmd["camera_object_points"][(i*7)+4].as<double>();
      load_transform.rotation.z = cmd["camera_object_points"][(i*7)+5].as<double>();
      load_transform.rotation.w = cmd["camera_object_points"][(i*7)+6].as<double>();
      
      collect_points::camera_object_points.push_back(load_transform);
    }
  }
}


namespace visp_hand2eye_calibration
{
Client::Client()
{
  camera_object_publisher_
      = n_.advertise<geometry_msgs::Transform> (visp_hand2eye_calibration::camera_object_topic, 1000);
  world_effector_publisher_
      = n_.advertise<geometry_msgs::Transform> (visp_hand2eye_calibration::world_effector_topic, 1000);

  reset_service_
      = n_.serviceClient<visp_hand2eye_calibration::reset> (visp_hand2eye_calibration::reset_service);
  compute_effector_camera_service_
      = n_.serviceClient<visp_hand2eye_calibration::compute_effector_camera> (
                                                                                      visp_hand2eye_calibration::compute_effector_camera_service);
  compute_effector_camera_quick_service_
      = n_.serviceClient<visp_hand2eye_calibration::compute_effector_camera_quick> (
                                                                                            visp_hand2eye_calibration::compute_effector_camera_quick_service);
}

void Client::initAndSimulate()
{
  ROS_INFO("Waiting for topics...");
  ros::Duration(1.).sleep();
  while(!reset_service_.call(reset_comm)){
    if(!ros::ok()) return;
    ros::Duration(1).sleep();
  }


  // We want to calibrate the hand to eye extrinsic camera parameters from 6 couple of poses: cMo and wMe
  const int N = 6;
  // Input: six couple of poses used as input in the calibration proces
  vpHomogeneousMatrix cMo; // eye (camera) to object transformation. The object frame is attached to the calibrartion grid
  vpHomogeneousMatrix wMe; // world to hand (end-effector) transformation
  vpHomogeneousMatrix eMc; // hand (end-effector) to eye (camera) transformation

  // Initialize an eMc transformation used to produce the simulated input transformations cMo and wMe
  vpTranslationVector etc(0.1, 0.2, 0.3);
  vpThetaUVector erc;
  erc[0] = vpMath::rad(10); // 10 deg
  erc[1] = vpMath::rad(-10); // -10 deg
  erc[2] = vpMath::rad(25); // 25 deg

  eMc.buildFrom(etc, erc);
  ROS_INFO("1) GROUND TRUTH:");

  ROS_INFO_STREAM("hand to eye transformation: " <<std::endl<<visp_bridge::toGeometryMsgsTransform(eMc)<<std::endl);

  vpColVector v_c(6); // camera velocity used to produce 6 simulated poses
  
  string file_path = package_path+"/data/calib_points.yaml"; 
  
  collect_points collected_points(file_path);
  
  for (int i = 0; i < N; i++)
  {
    
    geometry_msgs::Transform pose_c_o;
    pose_c_o = collected_points.camera_object_points[i];
    geometry_msgs::Transform pose_w_e;
    pose_w_e = collected_points.world_effector_points[i];

    camera_object_publisher_.publish(pose_c_o);
    world_effector_publisher_.publish(pose_w_e);

    emc_quick_comm.request.camera_object.transforms.push_back(pose_c_o);
    emc_quick_comm.request.world_effector.transforms.push_back(pose_w_e);

  }
  ros::Duration(1.).sleep();

}

void Client::computeUsingQuickService()
{
  vpHomogeneousMatrix eMc;
  vpThetaUVector erc;
  ROS_INFO("2) QUICK SERVICE:");
  if (compute_effector_camera_quick_service_.call(emc_quick_comm))
  {
    ROS_INFO_STREAM("hand_camera: "<< std::endl << emc_quick_comm.response.effector_camera);
  }
  else
  {
    ROS_ERROR("Failed to call service");
  }
}

void Client::computeFromTopicStream()
{
  vpHomogeneousMatrix eMc;
  vpThetaUVector erc;
  ROS_INFO("3) TOPIC STREAM:");
  if (compute_effector_camera_service_.call(emc_comm))
  {
    ROS_INFO_STREAM("hand_camera: " << std::endl << emc_comm.response.effector_camera);
  }
  else
  {
    ROS_ERROR("Failed to call service");
  }

}
}

/*
 * Local variables:
 * c-basic-offset: 2
 * End:
 */
