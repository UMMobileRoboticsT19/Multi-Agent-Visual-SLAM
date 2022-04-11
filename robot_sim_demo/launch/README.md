#ORBSLAM
## Launch the world
```bash
cd catkin_slam/
source devel/setup.bash
roslaunch robot_sim_demo robots_spawn.launch
```
## Launch ORBSLAM for robot1
```bash
cd ORB_SLAM2/
export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:/home/goro/ORB_SLAM2/Examples/ROS
ROS_NAMESPACE=robot1 rosrun ORB_SLAM2 RGBD Vocabulary/ORBvoc.bin TUM1.yaml /cam0/rgb/image_raw:=/robot1/cam0/rgb/image_raw /cam0/depth/image_raw:=/robot1/cam0/depth/image_raw
```
## Launch ORBSLAM for robot2
```bash
cd ORB_SLAM2/
export ROS_PACKAGE_PATH=${ROS_PACKAGE_PATH}:/home/goro/ORB_SLAM2/Examples/ROS
ROS_NAMESPACE=robot2 rosrun ORB_SLAM2 RGBD Vocabulary/ORBvoc.bin TUM1.yaml /cam0/rgb/image_raw:=/robot2/cam0/rgb/image_raw /cam0/depth/image_raw:=/robot2/cam0/depth/image_raw
```

## Launch cloud_to_grid node
```
ROS_NAMESPACE=robot1 roslaunch cloud_to_grid rgbd.launch
ROS_NAMESPACE=robot2 roslaunch cloud_to_grid rgbd.launch
```

## Saving a map
rosrun map_server map_saver -f single /map:=/robot1/map
rosrun map_server map_saver -f multiagents

```