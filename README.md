# Multi-Agent-Visual-SLAM

This is the code repository for Team 19 of Winter 2022 ROB530:Mobile Robotics Final Project.
We proposed a method for running Multi-Agent Visual SLAM in real-time in a simulation environment in Gazebo.


## How to setup the environment

### 1. Git clone this package to home directory

### 2. Build ORB_SLAM2


### 3. Build the rest ROS packages

We have three ROS packages:
`robot_sim_demo`, `cloud_to_grid`, and `merge_maps`.
Put these 3 folders under `~/catkin_ws/src/` and run **catkin_make** in `~/catkin_ws_/` folder.
You should be able to run all ROS nodes in these packages!


## How to run the project:

### 1. Open a terminal and run:
```
roslaunch robot_sim_demo robot_spawn.launch
```

### 2. Open other two terminals, run below commands. (Each command use one terminal)
```
ROS_NAMESPACE=robot1 rosrun ORB_SLAM2 RGBD Vocabulary/ORBvoc.bin TUM1.yaml /cam0/rgb/image_raw:=/robot1/cam0/rgb/image_raw /cam0/depth/image_raw:=/robot1/cam0/depth/image_raw

// Open another terminal

ROS_NAMESPACE=robot2 rosrun ORB_SLAM2 RGBD Vocabulary/ORBvoc.bin TUM1.yaml /cam0/rgb/image_raw:=/robot2/cam0/rgb/image_raw /cam0/depth/image_raw:=/robot2/cam0/depth/image_raw

```
This step will launch ORB_SLAM nodes, one for each robot.
Each SLAM nodes open 3 windows:
1. Keyframe window
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
