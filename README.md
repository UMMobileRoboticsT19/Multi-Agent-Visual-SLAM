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
