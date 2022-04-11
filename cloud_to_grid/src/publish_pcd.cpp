#include <ros/ros.h>
#include <pcl/point_cloud.h>
#include <pcl_conversions/pcl_conversions.h>
#include <sensor_msgs/PointCloud2.h>
#include <pcl/io/pcd_io.h>
#include <iostream>

main(int argc, char **argv)
{
    if(argc != 2)
    {
        std::cerr << std::endl << "Usage: rosrun cloud_to_grid publish_pcd path_to_pcd" << std::endl;
        ros::shutdown();
        return 1;
    }
    ros::init (argc, argv, "publish_pcd");

    ros::NodeHandle nh;
    ros::Publisher pcl_pub = nh.advertise<sensor_msgs::PointCloud2> ("pcl_output", 1);

    sensor_msgs::PointCloud2 output;
    pcl::PointCloud<pcl::PointXYZRGBA> cloud;

    pcl::io::loadPCDFile (argv[1], cloud);

    pcl::toROSMsg(cloud, output);
    output.header.frame_id = "map";

    ros::Rate loop_rate(1);
    while (ros::ok())
    {
        pcl_pub.publish(output);
        ros::spinOnce();
        loop_rate.sleep();
    }
    return 0;
}


