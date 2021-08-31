#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>
#include <tf/tf.h>

using namespace std;

ros::Publisher tcp_pose_pub;

void tcp_pose_publish(){
  
  ros::Rate rate(10.0);
  
  tf::TransformListener tf_listener;
  tf::StampedTransform transform;

  
  bool tf_not_ready = true;
  int ntries = 0;  
  
  while (tf_not_ready){
    
    
    static tf::TransformBroadcaster br;
    static tf::TransformListener tf_listener;
    tf::Transform transformation;
    transformation.setOrigin( tf::Vector3(0.190,-0.075, 0) );
    tf::Quaternion q;
    q.setRPY(0, 0, 0);
    transformation.setRotation(q);
    br.sendTransform(tf::StampedTransform(transformation, ros::Time::now(),"tool0", "tool_offset"));
    
    try {
      tf_not_ready = false; 
      
      tf_listener.lookupTransform( "tool_offset", "world", ros::Time(0), transform);
	    
    } 
    catch (tf::TransformException ex) { //do nothing
	tf_not_ready = true;
	ntries++;
	cout<<"waiting for "<<"lol"<<" frame; ntries = "<<ntries<<std::endl;
	ros::Duration(0.1).sleep();
     }
  
  
    geometry_msgs::PoseStamped tcp_pose;
    
    tcp_pose.header.stamp = ros::Time::now();
    tcp_pose.header.frame_id = "world";
    tcp_pose.pose.position.x = transform.getOrigin().getX();
    tcp_pose.pose.position.y = transform.getOrigin().getY();
    tcp_pose.pose.position.z = transform.getOrigin().getZ();
    tcp_pose.pose.orientation.x = transform.getRotation().x();
    tcp_pose.pose.orientation.y = transform.getRotation().y();
    tcp_pose.pose.orientation.z = transform.getRotation().z();
    tcp_pose.pose.orientation.w = transform.getRotation().w();
    tcp_pose_pub.publish(tcp_pose);
    rate.sleep();
  }
}


int main(int argc, char** argv){
  ros::init(argc, argv, "tcp_pose");

  ros::NodeHandle node;

  
  tcp_pose_pub = 
    node.advertise<geometry_msgs::PoseStamped>("/tcp_pose", 30);

  
  
  

  while(ros::ok()){
    tcp_pose_publish();
  }
  
  return 0;
};
