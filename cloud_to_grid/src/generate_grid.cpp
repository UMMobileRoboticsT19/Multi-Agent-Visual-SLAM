#include <ros/ros.h>
#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
#include <sensor_msgs/PointCloud2.h>
#include <pcl_conversions/pcl_conversions.h>
#include <cloud_to_grid/Grid_Tool.h>
#include <nav_msgs/OccupancyGrid.h>
#include <dynamic_reconfigure/server.h>
#include <cloud_to_grid/cloud_to_gridConfig.h>
using namespace std;

void TimerCallBack(const ros::TimerEvent&);
void CloudCallBack(const sensor_msgs::PointCloud2ConstPtr ros_cloud);

MyTool::MyGrid *mygrid;
MyTool::MapMetaData mapData;

ros::Publisher mappub;
ros::Subscriber cloud_sub;

nav_msgs::OccupancyGrid grid;
int map_counter=0;


void callback(cloud_to_grid::cloud_to_gridConfig &config, uint32_t level) {
    ROS_INFO("Reconfigure Request");
    MyTool::param.mode = config.mode;
    MyTool::param.project_direction = config.project_direction;
    MyTool::param.searchRadius=config.searchRadius;
    MyTool::param.deviation=config.deviation;
    MyTool::param.sigma=config.sigma;
    MyTool::param.threshold=config.threshold;
    MyTool::param.cellResolution=config.cellResolution;
    MyTool::param.clip_max=config.clip_max;
    MyTool::param.clip_min=config.clip_min;
    MyTool::param.map_size=config.map_size;
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "generate_grid_node");
    ros::start();

    mygrid=new MyTool::MyGrid();
    ros::NodeHandle nh;
    cloud_sub= nh.subscribe("pcl_output", 1, CloudCallBack);//need to change by yourself
    mappub= nh.advertise<nav_msgs::OccupancyGrid>("map", 1);
    ros::Timer timer = nh.createTimer(ros::Duration(0.5), TimerCallBack);

    dynamic_reconfigure::Server<cloud_to_grid::cloud_to_gridConfig> server;
    dynamic_reconfigure::Server<cloud_to_grid::cloud_to_gridConfig>::CallbackType f;

    f = boost::bind(&callback, _1, _2);
    server.setCallback(f);

    ros::spin();
    return 0;
}
void TimerCallBack(const ros::TimerEvent&)
{
    if(!mapData.data.empty())
    {
        grid.header.seq = map_counter++;
        grid.header.frame_id = "odom";
        grid.info.origin.position.z = 0;
        grid.info.origin.orientation.w = 1;
        grid.info.origin.orientation.x = 0;
        grid.info.origin.orientation.y = 0;
        grid.info.origin.orientation.z = 0;

        grid.header.stamp.sec = ros::Time::now().sec;
        grid.header.stamp.nsec = ros::Time::now().nsec;
        grid.info.map_load_time = ros::Time::now();
        grid.info.resolution = mapData.resolution;
        grid.info.width = mapData.width;
        grid.info.height = mapData.height;
        grid.info.origin.position.x = mapData.map_length/2*-1;  //minx
        grid.info.origin.position.y = mapData.map_length/2*-1;  //miny
        grid.data = mapData.data;
        mappub.publish(grid);
        std::cout<<"Pub grid!!"<< std::endl;
    }

}

void CloudCallBack(const sensor_msgs::PointCloud2ConstPtr ros_cloud){
    pcl::PCLPointCloud2 pcl_pc2;
    pcl_conversions::toPCL(*ros_cloud,pcl_pc2);
    MyTool::PointCloud::Ptr temp_cloud(new MyTool::PointCloud());
    pcl::fromPCLPointCloud2(pcl_pc2,*temp_cloud);
    mygrid->update(temp_cloud,mapData);
}


