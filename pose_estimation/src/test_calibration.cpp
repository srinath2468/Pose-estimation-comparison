#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <tf/tf.h>

using namespace std;

ros::Publisher tcp_pose_pub;

/*translation: 
  x: 1.32269
  y: 0.0323465
  z: -0.549246
rotation: 
  x: 0.579992
  y: 0.459502
  z: 0.52642
  w: 0.418748*/

/*translation: 
  x: -2.57314
  y: -0.0614327
  z: -0.418541
rotation: 
  x: 0.183068
  y: -0.0108298
  z: 0.063095
  w: 0.981014*/


void tcp_pose_publish(){
  
  ros::Rate rate(10.0);
  
  tf::TransformListener tf_listener;
  tf::StampedTransform transform;

  
  double x = 0.421549;
  double y = 0.306726;
  double z = 1.00529;
  
//   double roll=0, pitch=-M_PI/2, yaw=0;

  double ox= -0.629818;
  double oy= 0.627742;
  double oz= -0.333692;
  double ow= 0.312922;
  
// translation: 
//   x: 0.421549
//   y: 0.306726
//   z: 1.00529
// rotation: 
//   x: -0.629818
//   y: 0.627742
//   z: -0.333692
//   w: 0.312922
 
  double roll, pitch, yaw;
  
  

    
    
  static tf::TransformBroadcaster br;
  tf::Transform transformation;
  transformation.setOrigin( tf::Vector3(x, y, z) );
  tf::Quaternion q;
  q.setX(ox);
  q.setY(oy);
  q.setZ(oz);
  q.setW(ow);
//   q.normalize();
//   tf::Matrix3x3(q).getRPY(roll, pitch, yaw);
  
//   q.setRPY(roll, pitch, yaw);
  transformation.setRotation(q);
//   br.sendTransform(tf::StampedTransform(transformation, ros::Time::now() ,"zed_left_camera_frame", "ur10_robot_base"));
  br.sendTransform(tf::StampedTransform(transformation, ros::Time::now() , "ur10_robot_base" ,"zed_left_camera_frame"));
    
    rate.sleep();
  
}


int main(int argc, char** argv){
  ros::init(argc, argv, "calibration_test");

  ros::NodeHandle node;
  
  
  

  while(ros::ok()){
    tcp_pose_publish();
  }
  
  return 0;
};