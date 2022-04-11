#include <ros/ros.h>
#include<iostream>
#include<algorithm>
#include<fstream>
#include<chrono>
// #include <sensor_msgs/PointCloud2.h>
// #include <pcl_conversions/pcl_conversions.h>
// #include <cloud_to_grid/Grid_Tool.h>
// #include <nav_msgs/OccupancyGrid.h>
// #include <dynamic_reconfigure/server.h>
// #include <cloud_to_grid/cloud_to_gridConfig.h>


#include "nav_msgs/OccupancyGrid.h"
#include "std_msgs/Header.h"
#include "nav_msgs/MapMetaData.h"

using namespace std;

void TimerCallBack(const ros::TimerEvent&);
// void CloudCallBack(const sensor_msgs::PointCloud2ConstPtr ros_cloud);



ros::Publisher final_map_pub;
ros::Subscriber map1_sub;
ros::Subscriber map2_sub;

nav_msgs::OccupancyGrid grid1;
nav_msgs::OccupancyGrid grid2;
nav_msgs::OccupancyGrid grid;
int map_counter=0;

// Store /map1 data to grid1
void map1Callback(const nav_msgs::OccupancyGrid::ConstPtr& msg){
    cout<<"map1 callback\n";
    grid1.header = msg->header;
    grid1.info = msg->info;
    grid1.data = msg->data;
}

// Store /map2 data to grid2
void map2Callback(const nav_msgs::OccupancyGrid::ConstPtr& msg){
    cout<<"map2 callback\n";
    grid2.header = msg->header;
    grid2.info = msg->info;
    // ROS_INFO("Got map %d %d", info.width, info.height);
    // Map map(info.width, info.height);
    grid2.data = msg->data;
}





#define WORLD_SIZE 400 // 400 * 400 grids
#define MAP_RESOLUTION 0.05 
#define GLOBAL_RESOLUTION 0.05
#define GLOBAL_WIDTH 800
#define GLOBAL_HEIGHT 800
#define X_OFFSET GLOBAL_WIDTH/2
#define Y_OFFSET GLOBAL_HEIGHT/2

#include <Eigen/Dense>
#include <Eigen/Geometry>
using namespace Eigen;


/*
Matrix4f create_affine_matrix(float a, float b, float c, Vector3f trans)
{
    Transform<float, 3, Eigen::Affine> t;
    t = Translation<float, 3>(trans);
    t.rotate(AngleAxis<float>(a, Vector3f::UnitX()));
    t.rotate(AngleAxis<float>(b, Vector3f::UnitY()));
    t.rotate(AngleAxis<float>(c, Vector3f::UnitZ()));
    return t.matrix();
}
MatrixXf  robot1_transform(3, 3);
MatrixXf  robot2_transform(3, 3);
*/

MatrixXd robot1_transform(3,3);
MatrixXd robot2_transform(3,3);


/*
    Subscibe:  /map1
    Subscribe: /map2

    Publish:   /final_map

    nav_msgs/OccupancyGrid.msg

    std_msgs/Header header
    nav_msgs/MapMetaData info
    int8[] data

*/

int main(int argc, char **argv)
{
    ros::init(argc, argv, "merge_map_node");
    ros::start();

    // mygrid=new MyTool::MyGrid();
    ros::NodeHandle nh;
    map1_sub = nh.subscribe("map1", 1, map1Callback);//need to change by yourself
    map2_sub = nh.subscribe("map2", 1, map2Callback);
    final_map_pub= nh.advertise<nav_msgs::OccupancyGrid>("map", 1);

    // Publish merged_map her
    ros::Timer timer = nh.createTimer(ros::Duration(0.5), TimerCallBack);

    // dynamic_reconfigure::Server<cloud_to_grid::cloud_to_gridConfig> server;
    // dynamic_reconfigure::Server<cloud_to_grid::cloud_to_gridConfig>::CallbackType f;
    // f = boost::bind(&callback, _1, _2);
    // server.setCallback(f);

    ////////////////////////////////////////////////////////////////////
    // Init robot pose (in radians, meters)
    float robot1_x = 4.5;
    float robot1_y = 4.5;
    float robot1_theta = 0;
    float robot2_x = 5;
    float robot2_y = 5;
    float robot2_theta = 0;


    // TODO: transformation matrix should be in "number_of_grids" unit!
    // meters => divide by 0.05
    robot1_transform << cos(robot1_theta), -sin(robot1_theta), robot1_x/GLOBAL_RESOLUTION,
                        sin(robot1_theta), cos(robot1_theta), robot1_y/GLOBAL_RESOLUTION,
                        0, 0, 1;
    cout<<"robot1 transformation matrix: \n";                        
    std::cout << robot1_transform<<endl;
    robot2_transform << cos(robot2_theta), -sin(robot2_theta), robot2_x/GLOBAL_RESOLUTION,
                        sin(robot2_theta), cos(robot2_theta), robot2_y/GLOBAL_RESOLUTION,
                        0, 0, 1;
    cout<<"robot2 transformation matrix: \n";
    std::cout << robot2_transform<<endl;

    /////////////////////////////////////////////////////////////////////
    // Initialized grid.size()
    grid.data.resize(GLOBAL_WIDTH * GLOBAL_HEIGHT);
    // grid.header.seq = map_counter++;
    grid.header.frame_id = "odom";
    grid.info.origin.position.z = 0;
    grid.info.origin.orientation.w = 1;
    grid.info.origin.orientation.x = 0;
    grid.info.origin.orientation.y = 0;
    grid.info.origin.orientation.z = 0;

    // grid.header.stamp.sec = ros::Time::now().sec;
    // grid.header.stamp.nsec = ros::Time::now().nsec;
    // grid.info.map_load_time = ros::Time::now();
    grid.info.resolution = GLOBAL_RESOLUTION;
    grid.info.width = GLOBAL_WIDTH;
    grid.info.height = GLOBAL_HEIGHT;
    // grid.info.origin.position.x = mapData.map_length/2*-1;  //minx
    // grid.info.origin.position.y = mapData.map_length/2*-1;  //miny
    // grid.data = mapData.data;

    ros::MultiThreadedSpinner spinner;
    spinner.spin();

    // ros::spin();
    return 0;
}



/*

TODO:
1. map resolution: 0.05m (5cm/per grid)
2. map width: 200 grids ( 10m )
3. Convert grid1.loc(x, y) => global.loc(x,y)
4. Offset global.loc  x: add half_wdith (200 grids)
                      y: add half_height
   (Since the world frame origin is in the middle of the world map)
*/


/*
Input: pixel locations x, y in local frame (integers)
Output:pixel locations x, y in global frame (integers might be negative)
*/


std::pair<int, int> grid1_to_grid(int x, int y){
    std::pair<int, int>grid_loc;

    Vector3d grid1_loc(3), global_loc(3);
    grid1_loc << x, y, 1;
    global_loc = robot1_transform.inverse() * grid1_loc;
    grid_loc.first = int(global_loc[0] + X_OFFSET);
    grid_loc.second = int(global_loc[1] +Y_OFFSET);
    // cout<<"grid1 to global_loc "<<grid_loc.first << " "<<grid_loc.second<<endl;
    return grid_loc;
}

std::pair<int, int> grid2_to_grid(int x, int y){
    std::pair<int, int>grid_loc;

    Vector3d grid2_loc(3), global_loc(3);
    grid2_loc << x, y, 1;
    global_loc = robot2_transform.inverse() * grid2_loc;
    grid_loc.first = int(global_loc[0]+ X_OFFSET);
    grid_loc.second = int(global_loc[1]+ Y_OFFSET);
    // cout<<"grid2 to global_loc "<< grid_loc.first << " "<<grid_loc.second<<endl;

    return grid_loc;
}

// Q: when to updat mapData?
void TimerCallBack(const ros::TimerEvent&)
{
    cout<<"TimerCallBack\n";
    // cout<<"grid1.data.size() "<<grid1.data.size() << endl;
    // cout<<"grid2.data.size() "<<grid2.data.size() << endl;
    if(!grid1.data.empty() && !grid2.data.empty()){

        // cout<<"Update global grid!\n";        
        // vector<pair<bool, bool>> exist(grid.data.size(), make_pair(false, false));

        // cout<<"======== Info =================\n";
        // cout<<"grid1.info.width, height "<<grid1.info.width<<" "<<grid1.info.height<<endl;
        // cout<<"grid2.info.width, height "<<grid2.info.width<<" "<<grid2.info.height<<endl;


        // // Transform grid1.data to grid.data
        // for(int i=0; i<grid1.data.size(); i++){
        //     int x = i%grid1.info.width;
        //     int y = i/grid1.info.width;
        //     std::pair<int, int> grid_loc = grid1_to_grid(x, y);
        //     if(grid_loc.first <0 or grid_loc.first>GLOBAL_WIDTH or grid_loc.second<0 or grid_loc.second >GLOBAL_HEIGHT){
        //         cout<<"Invalid global location! "<<grid_loc.first<<" "<<grid_loc.second<<endl;
        //     }
        //     else{
        //         // Valid pixel global coordinate
        //         grid.data[grid_loc.first + grid.info.width*grid_loc.second] = grid1.data[i];
        //         exist[grid_loc.first + grid.info.width*grid_loc.second].first = true;
        //     }
        // }

        // for(int j=0; j<grid2.data.size(); j++){
        //     int x = j%grid2.info.width;
        //     int y = j/grid2.info.width;            
        //     std::pair<int, int> grid_loc = grid2_to_grid(x, y);
        //     if(grid_loc.first <0 or grid_loc.first>GLOBAL_WIDTH or grid_loc.second<0 or grid_loc.second >GLOBAL_HEIGHT){
        //         cout<<"Invalid global location! "<<grid_loc.first<<" "<<grid_loc.second<<endl;
        //     }
        //     else{

        //         grid.data[grid_loc.first + grid.info.width*grid_loc.second] = grid2.data[j];
        //         exist[grid_loc.first + grid.info.width*grid_loc.second].second = true;
        //     }
        // }

        // Merge map1 and map2 algorithm        
        // for(int i=0; i< grid.data.size(); ++i){
        //     // query the 'exist_vector'
        //     // exist_vector is a vector of pair<bool, bool>
        //     // True: grid1 has this index in its map
        //     // False: 
        //     if(exist[i].first==true && exist[i].second==true){
        //         if(grid1.data[i]==0.5 && grid2.data[i]!=0.5){
        //             grid.data[i] = grid2.data[i];
        //         }
        //         else if(grid1.data[i]!=0.5 && grid2.data[i]==0.5){
        //             grid.data[i] = grid1.data[i];
        //         }
        //         else if(grid1.data[i]==0.5 && grid2.data[i]==0.5){
        //             grid.data[i] = 0.5; // unknown
        //         }
        //         else{
        //             grid.data[i] = (grid1.data[i] + grid2.data[i])/2 ;

        //         }
        //     }
        //     // Else, do nothing!
        // }

        // Transform grid2.data to grid.data
        cout<<"grid1.data.size() "<<grid1.data.size()<<endl;
        cout<<"grid2.data.size() "<<grid2.data.size()<<endl;
        int mapSize = min(grid1.data.size(), grid2.data.size());
        // cout<< (grid.data.size()) <<endl;

        grid.data.resize(mapSize);
        for(int i=0; i< mapSize; ++i){
            if(grid1.data[i]==-1 && grid2.data[i]!=-1){
                grid.data[i] = grid2.data[i];
            }
            else if(grid1.data[i]!=-1 && grid2.data[i]==-1){
                grid.data[i] = grid1.data[i];
            }
            else if(grid1.data[i]==-1 && grid2.data[i]==-1){
                grid.data[i] = -1; // unknown
            }
            else{
                grid.data[i] = (grid1.data[i] + grid2.data[i])/2 ;

            }
        }

        final_map_pub.publish(grid);
        std::cout<<"Pub grid!!"<< std::endl;

    }
}




