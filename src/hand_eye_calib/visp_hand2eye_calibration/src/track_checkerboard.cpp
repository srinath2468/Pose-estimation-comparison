
#define _USE_MATH_DEFINES

#include <iostream>
#include <ros/ros.h>
#include <pcl_ros/point_cloud.h>
#include <pcl/point_types.h>
#include <message_filters/subscriber.h>
#include <message_filters/synchronizer.h>
#include <message_filters/sync_policies/approximate_time.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/core/core.hpp"
#include <pcl/common/io.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/surface/mls.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <pcl/kdtree/kdtree_flann.h>
#include <pcl/surface/mls.h>
#include <limits>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/filters/filter.h>
#include <algorithm>
#include <pcl/features/normal_3d.h>
#include <tf_conversions/tf_eigen.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/PoseStamped.h>
#include <math.h>
#include "opencv2/calib3d.hpp"
#include "opencv2/core/eigen.hpp"
#include "eigen_conversions/eigen_msg.h"
#include <Eigen/Core>
#include <tf_conversions/tf_eigen.h>
#include <tf/transform_listener.h>
#include <tf/transform_broadcaster.h>

using namespace sensor_msgs;
using namespace message_filters;
using namespace cv;
using namespace std;


typedef pcl::PointXYZ PointP;
typedef pcl::PointCloud<PointP> PointCloudP;
typedef pcl::PointXYZRGB PointT;
typedef pcl::PointCloud<PointT> PointCloudT;
typedef pcl::PointNormal PointNT;
typedef pcl::PointCloud<PointNT> PointNCloudT;
typedef pcl::PointXYZL PointLT;
typedef pcl::PointCloud<PointLT> PointLCloudT;
typedef pcl::PointXYZI PointIT;
typedef pcl::PointCloud<PointIT> PointICloudT;

ros::Publisher cam_obj_pose_pub;
ros::Publisher check_pose_pub;
void messageCallBack(const sensor_msgs::ImageConstPtr& rgb_img, const sensor_msgs::PointCloud2::ConstPtr& in_PCL, const sensor_msgs::CameraInfo::ConstPtr& camera_info);
std::vector<cv::Point3f> Create3DChessboardCorners(cv::Size boardSize, float squareSize);
tf::StampedTransform transform_default_to_camera_orientation;
tf::StampedTransform transform_to_rotate_about_x;

int main(int argc, char** argv) {
  
  ros::init(argc, argv, "pick_points_node");
  
  ros::NodeHandle nh;
  
  ros::NodeHandle nh_priv("~");
  
  string img_topic_name,pcl_topic_name,cam_info_topic_name;
  
  nh_priv.getParam("img_topic_name", img_topic_name);
  nh_priv.getParam("pcl_topic_name", pcl_topic_name);
  nh_priv.getParam("cam_info_topic_name", cam_info_topic_name);
  
  
  message_filters::Subscriber<Image> rgb_img(nh,img_topic_name, 1);
  message_filters::Subscriber<sensor_msgs::PointCloud2> in_PCL(nh,pcl_topic_name, 1);
  message_filters::Subscriber<sensor_msgs::CameraInfo> cam_info(nh,cam_info_topic_name, 1);
  
  typedef sync_policies::ApproximateTime<Image, sensor_msgs::PointCloud2, sensor_msgs::CameraInfo> SyncPolicy;
  Synchronizer<SyncPolicy> sync(SyncPolicy(25), rgb_img, in_PCL, cam_info);
  
  sync.registerCallback(boost::bind(&messageCallBack, _1, _2, _3));
  
  static tf::TransformBroadcaster br;
  static tf::TransformListener tf_listener;
  tf::Transform transform;
  transform.setOrigin( tf::Vector3(0.0, 0.0, 0.0) );
  tf::Quaternion q;
  q.setRPY(0, -M_PI/2, M_PI/2);
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(),"map", "map_reoriented"));
  
  bool tf1_not_ready = true;
  int ntries = 0;  
  
  while (tf1_not_ready){
  try {
    tf1_not_ready = false;            
    tf_listener.lookupTransform("map_reoriented", "map", ros::Time(0), transform_default_to_camera_orientation);
//     tf_listener.lookupTransform( "map","map_reoriented", ros::Time(0), transform_default_to_camera_orientation);
    
  } catch (tf::TransformException ex) { //do nothing
    tf1_not_ready = true;
    ntries++;      
    cout<<"waiting for reorientation frame; "<< ntries <<std::endl;
    ros::Duration(0.1).sleep();
    }
  }   
  
  tf1_not_ready = false;
  
  
  transform.setOrigin( tf::Vector3(0.0, 0.0, 0.0) );
  q.setRPY(M_PI/2, 0, 0);
  transform.setRotation(q);
  br.sendTransform(tf::StampedTransform(transform, ros::Time::now(),"map", "map_x"));
  
  while (tf1_not_ready){
  try {
    tf1_not_ready = false;            
    tf_listener.lookupTransform("map_x", "map", ros::Time(0), transform_to_rotate_about_x);
//     tf_listener.lookupTransform( "map","map_reoriented", ros::Time(0), transform_default_to_camera_orientation);
    
  } catch (tf::TransformException ex) { //do nothing
    tf1_not_ready = true;
    ntries++;      
    cout<<"waiting for reorientation frame; "<< ntries <<std::endl;
    ros::Duration(0.1).sleep();
    }
  }
  
  cam_obj_pose_pub = nh_priv.advertise<geometry_msgs::PoseStamped>("camera_object_pose", 1000);
  check_pose_pub = nh_priv.advertise<geometry_msgs::PoseStamped>("check_pose", 1000);
  
  
  ros::spin();

  return 0;
}
 
std::vector<cv::Point3f> Create3DChessboardCorners(cv::Size boardSize, float squareSize)
{
  // This function creates the 3D points of your chessboard in its own coordinate system
 
  std::vector<cv::Point3f> corners;
 
  for( int i = 0; i < boardSize.height; i++ )
  {
    for( int j = 0; j < boardSize.width; j++ )
    {
      corners.push_back(cv::Point3f(float(j*squareSize),
                                float(i*squareSize),0));
      
      
    }
  }
 
  return corners;
}


geometry_msgs::PoseStamped rotate_about_y(geometry_msgs::PoseStamped poseStamped){
   
  geometry_msgs::PoseStamped output;
  tf::Quaternion q;
  q.setRPY(0,-M_PI/2,0);
  
  Eigen::Affine3d transform_frame; 
  
  tf::StampedTransform transform;
  
  transform.setOrigin(tf::Vector3(0,0,0));
  
  transform.setRotation(q);
  
  tf::transformTFToEigen(transform, transform_frame);  
  
  Eigen::Affine3d transform_frame_3d;
  Eigen::Affine3d target_wrt_camera_3d;
  
  tf::poseMsgToEigen(poseStamped.pose, target_wrt_camera_3d);
    
  transform_frame_3d = transform_frame * target_wrt_camera_3d;
  
  tf::poseEigenToMsg(transform_frame_3d,output.pose);  
  
  
  float temp = output.pose.position.x;
  
  output.pose.position.x = -output.pose.position.z;
  output.pose.position.z = temp;
  
  output.header.stamp = ros::Time::now();
  output.header.frame_id = "map";
  
  return(output);

}

geometry_msgs::PoseStamped rotate_about_x(geometry_msgs::PoseStamped poseStamped){
   
   Eigen::Affine3d transform_frame; 
  
  geometry_msgs::PoseStamped output;
  
  tf::transformTFToEigen(transform_to_rotate_about_x, transform_frame);  
  
  Eigen::Affine3d transform_frame_3d;
  Eigen::Affine3d target_wrt_camera_3d;
  
  tf::poseMsgToEigen(poseStamped.pose, target_wrt_camera_3d);
    
  transform_frame_3d = transform_frame * target_wrt_camera_3d;
  
  tf::poseEigenToMsg(transform_frame_3d,output.pose);
  
//   float temp = output.pose.position.x;
//   
//   output.pose.position.x = -output.pose.position.z;
//   output.pose.position.z = temp;
  
  output.header.stamp = ros::Time::now();
  output.header.frame_id = "map";
  
  
  return(output);

}

geometry_msgs::PoseStamped reorient_frame(geometry_msgs::PoseStamped poseStamped){
   
  Eigen::Affine3d transform_frame; 
  
  geometry_msgs::PoseStamped output;
  
  tf::transformTFToEigen(transform_default_to_camera_orientation, transform_frame);  
  
  Eigen::Affine3d transform_frame_3d;
  Eigen::Affine3d target_wrt_camera_3d;
  
  tf::poseMsgToEigen(poseStamped.pose, target_wrt_camera_3d);
    
  transform_frame_3d = transform_frame * target_wrt_camera_3d;
  
  tf::poseEigenToMsg(transform_frame_3d,output.pose);
  
//   float temp = output.pose.position.x;
//   
//   output.pose.position.x = -output.pose.position.z;
//   output.pose.position.z = temp;
  
  output.header.stamp = ros::Time::now();
  output.header.frame_id = "map";
  
  
  return(output);
  
  

}


bool isRotationMatrix(Mat &R)
{
    Mat Rt;
    transpose(R, Rt);
    Mat shouldBeIdentity = Rt * R;
    Mat I = Mat::eye(3,3, shouldBeIdentity.type());
     
    return  norm(I, shouldBeIdentity) < 1e-6;
     
}

Vec3f rotationMatrixToEulerAngles(Mat &R)
{
 
    assert(isRotationMatrix(R));
     
    float sy = sqrt(R.at<double>(0,0) * R.at<double>(0,0) +  R.at<double>(1,0) * R.at<double>(1,0) );
 
    bool singular = sy < 1e-6; // If
 
    float x, y, z;
    if (!singular)
    {
        x = atan2(R.at<double>(2,1) , R.at<double>(2,2));
        y = atan2(-R.at<double>(2,0), sy);
        z = atan2(R.at<double>(1,0), R.at<double>(0,0));
    }
    else
    {
        x = atan2(-R.at<double>(1,2), R.at<double>(1,1));
        y = atan2(-R.at<double>(2,0), sy);
        z = 0;
    }
    return Vec3f(x, y, z);
     
     
     
}


void messageCallBack(const sensor_msgs::ImageConstPtr& rgb_img, const sensor_msgs::PointCloud2::ConstPtr& in_PCL, const sensor_msgs::CameraInfo::ConstPtr& camera_info){
  
  PointCloudP::Ptr inCloud (new PointCloudP ());
  pcl::fromROSMsg (*in_PCL, *inCloud);
  
  //rgb image message
  Mat rgbImage = cv_bridge::toCvShare(rgb_img, "bgr8")->image.clone();
  
  Mat image;
  cvtColor(rgbImage, image, cv::COLOR_BGR2GRAY);
  
  cv::Size boardSize(7,6);
  vector<Point2f> corners;
  
  /// Set the neeed parameters to find the refined corners
  Size winSize = Size( 5, 5 );
  Size zeroZone = Size( -1, -1 );
  TermCriteria criteria = TermCriteria( CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 40, 0.001 );
  
  bool found = findChessboardCorners(image, boardSize, corners);
  
  if(!found)
    return;
  
  cornerSubPix( image, corners, winSize, zeroZone, criteria );
  
  Mat rvec, tvec;
  vector<Point3f> objectPoints;
  objectPoints = Create3DChessboardCorners(cv::Size(7,6),0.025);
  
  cv::Mat camera_matrix = (cv::Mat_<double>(3,3) << camera_info->K[0], camera_info->K[1], camera_info->K[2],
						    camera_info->K[3], camera_info->K[4], camera_info->K[5],
						    camera_info->K[6], camera_info->K[7], camera_info->K[8]);
  
  solvePnP(objectPoints, corners, camera_matrix, camera_info->D, rvec, tvec);
  
//   cout<<"rvec:"<<rvec<<endl;
  
  vector<Point3d> axes_points;
  vector<Point2d> projected_points;
  
  axes_points.push_back(Point3d(0.075,0,0));
  axes_points.push_back(Point3d(0,0.075,0));
  axes_points.push_back(Point3d(0,0,0.075));
  
//   project 3D points to image plane
  projectPoints(axes_points,rvec,tvec,camera_matrix,camera_info->D,projected_points);
  
  Point display_point0 = Point((int)projected_points[0].x, (int)projected_points[0].y);
  Point display_point1 = Point((int)projected_points[1].x, (int)projected_points[1].y);
  Point display_point2 = Point((int)projected_points[2].x, (int)projected_points[2].y);
  Point corner = Point(corners[24].x,corners[24].y);
  
  line(rgbImage,corner,display_point0,cv::Scalar(0,0,255),2);
  line(rgbImage,corner,display_point1,cv::Scalar(0,255,0),2);
  line(rgbImage,corner,display_point2,cv::Scalar(255,0,0),2);
  
//   line(rgbImage,corners[0],corners[5],cv::Scalar(0,0,0),2);
//   line(rgbImage,corners[0],corners[35],cv::Scalar(255,255,254),2);
  
  Mat R;
  
rvec.at<double>(0) = M_PI-rvec.at<double>(0);
rvec.at<double>(1) = rvec.at<double>(1);
rvec.at<double>(2) = M_PI+rvec.at<double>(2);

  cv::Rodrigues(rvec,R);
//   R = R.t();
//   tvec = -R*tvec;

  
  
  tf::Matrix3x3 rotations;
  
  rotations[0][0]= R.at<double>(0,0);
  rotations[0][1] = R.at<double>(0,2);
  rotations[0][2] = R.at<double>(0,1);
  
  rotations[2][0] = R.at<double>(2,0);
  rotations[1][1] = R.at<double>(2,2);
  rotations[1][2] = R.at<double>(2,1);
  
  rotations[2][0] = R.at<double>(1,0);
  rotations[2][1] = R.at<double>(1,2);
  rotations[2][2] = R.at<double>(1,1);

  imshow("img",rgbImage);
  
  geometry_msgs::PoseStamped camera_object_pose;
  
  camera_object_pose.header.stamp = ros::Time::now();
  
  double unit_distance_3d;
  
  int index = (((int)corners[24].y)*image.cols) + (int)corners[24].x;
  
  
  tf::Quaternion q = tf::createQuaternionFromRPY(rvec.at<double>(0),rvec.at<double>(1), rvec.at<double>(2));
  
  Eigen::Matrix3d mat;
  cv2eigen(R, mat);
  
//   Eigen::Quaterniond q(mat);
 
  camera_object_pose.pose.orientation.x = q.x();
  camera_object_pose.pose.orientation.y = q.y();
  camera_object_pose.pose.orientation.z = q.z();
  camera_object_pose.pose.orientation.w = q.w();
  
  
  camera_object_pose.header.frame_id = "map";
  camera_object_pose.header.stamp = ros::Time::now();
  
  
  
  geometry_msgs::PoseStamped reoriented_pose;
  
  reoriented_pose = reorient_frame(camera_object_pose);
  
  reoriented_pose.pose.position.z = inCloud->points[index].x;
  reoriented_pose.pose.position.y = -inCloud->points[index].z;
  reoriented_pose.pose.position.x = inCloud->points[index].y;
  
  cam_obj_pose_pub.publish(reoriented_pose);
  
  reoriented_pose.pose.position.x = inCloud->points[index].x;
  reoriented_pose.pose.position.z = inCloud->points[index].z;
  reoriented_pose.pose.position.y = inCloud->points[index].y;
  
  check_pose_pub.publish(reoriented_pose);
  
  
  
  cv::waitKey(1);
  
}
