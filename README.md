# Multi-Agent-Visual-SLAM

This is the code repository for Team 19 of Winter 2022 ROB530:Mobile Robotics Final Project.
We proposed a method for running Multi-Agent Visual SLAM in real-time in a simulation environment in Gazebo.


## How to setup the environment

### 1. Git clone this package to home directory

### 2. Build ORB_SLAM2
chmod +x build.sh build_ros.sh

### 3. Build the rest ROS packages

We have three ROS packages:
`robot_sim_demo`, `cloud_to_grid`, and `merge_maps`.
Put these 3 folders under `~/catkin_ws/src/` and run **catkin_make** in `~/catkin_ws_/` folder.
You should be able to run all ROS nodes in these packages!


## How to run the project:

### 1. Open a terminal and run:
if you want to launch both the world and multiple car models, run: 
'''
roslaunch robot_sim_demo robots_spawn.launch
'''
if you want to launch both the world and multiple car models, run: 
'''
roslaunch robot_sim_demo robot_single_spawn.launch
'''
if you want to launch just the car model in gazebo, run:
'''
roslaunch robot_sim_demo car.launch
'''
### 2. Open other two terminals, run below commands. (Each command use one terminal)
```
cd ORB_SLAM2/
export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:<YOUR PATH>/ORB_SLAM2/Examples/ROS
ROS_NAMESPACE=robot<NUM> rosrun ORB_SLAM2 RGBD Vocabulary/ORBvoc.bin TUM1.yaml /cam0/rgb/image_raw:=/robot<NUM>/cam0/rgb/image_raw /cam0/depth/image_raw:=/robot<NUM>/cam0/depth/image_raw

```
This step will launch ORB_SLAM nodes, one for each robot.
Each SLAM nodes open 3 windows:
1. Pose graph window
2. Point Clouds viewr
3. Features in camera view

### 3. Open another terminal
```
roslaunch robot_sim_demo cloud_to_grid.launch
```
This step open 4 nodes:
1. Two cloud_to_grid nodes
2. One merge_maps node
3. One rviz node

**rviz** is a visualzation tool of ROS. Click `Add` -> By Topics -> `/map`,
You should be able to view the map built by two robots in real time.
