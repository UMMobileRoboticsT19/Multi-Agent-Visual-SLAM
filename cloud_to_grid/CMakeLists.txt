cmake_minimum_required(VERSION 2.8.3)
project(cloud_to_grid)

## Compile as C++11, supported in ROS Kinetic and newer
add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  pcl_conversions
  pcl_ros
  image_transport
  cv_bridge
  dynamic_reconfigure
)


 generate_dynamic_reconfigure_options(
   config/cloud_to_grid.cfg
 )

###################################
## catkin specific configuration ##
###################################
## The catkin_package macro generates cmake config files for your package
## Declare things to be passed to dependent projects
## INCLUDE_DIRS: uncomment this if your package contains header files
## LIBRARIES: libraries you create in this project that dependent projects also need
## CATKIN_DEPENDS: catkin_packages dependent projects also need
## DEPENDS: system dependencies of this project that dependent projects also need
catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES cloud_to_grid
#  CATKIN_DEPENDS roscpp rospy std_msgs
#  DEPENDS system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include ${catkin_INCLUDE_DIRS} 
)

## Declare a C++ library
 add_library(Grid_Tool
 src/Grid_Tool.cpp
 )

target_link_libraries(Grid_Tool
   ${catkin_LIBRARIES}
 )



add_executable(generate_grid
  src/generate_grid.cpp
)
add_dependencies(generate_grid ${PROJECT_NAME}_gencfg ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(generate_grid Grid_Tool
  ${catkin_LIBRARIES}  )


add_executable(publish_pcd
  src/publish_pcd.cpp
)
add_dependencies(publish_pcd ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(publish_pcd
  ${catkin_LIBRARIES}
)
