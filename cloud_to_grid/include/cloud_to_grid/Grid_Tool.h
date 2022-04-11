#ifndef GRID_TOOL_H
#define GRID_TOOL_H

#include <iostream>
#include <cmath>
#include <map>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/common/common_headers.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/features/normal_3d.h>
#include <pcl/console/parse.h>
#include <thread>

#define DEBUG
using namespace std;
namespace MyTool
{
    typedef pcl::PointXYZRGBA PointType;
    typedef pcl::PointCloud<PointType> PointCloud;
    typedef pcl::PointCloud<pcl::Normal> NormalCloud;

    struct Param
    {
        bool mode=false; //false global;true increasing
        int project_direction = 2;//0 x;1 y;2 z
        double searchRadius=0.1;
        double deviation=0.5;
        double sigma=0.5;
        int threshold=5;
        double cellResolution=0.05;
        double clip_max=1.0;
        double clip_min=-1.0;
        double map_size=40;
        int border_grids=int(map_size/cellResolution);
    } param;

    struct Coordiate{
        int x;
        int y;
        Coordiate(){};
        Coordiate(int _x,int _y):x(_x),y(_y){
        }
        bool operator<(const Coordiate &coord) const
        {
            return (x < coord.x) || (x == coord.x && y < coord.y) ;
        }
    };
    struct MapMetaData{
        int width=0;
        int height=0;
        int xMin=0;
        int yMin=0;
        double map_length=0.0;
        double resolution=0.05;
        std::vector<signed char> data;
    };

    void GetGridCoord(const double temp_x,const double temp_y,int &x,int &y);
    void RemoveUnusedPoint(PointCloud::Ptr &cloud);

    class MyGrid
    {
     private:
          NormalCloud::Ptr cloudNormalPtr;
          bool isGlobalUpdate = false;
          std::map<Coordiate, int> gridPoints;
          std::vector<signed char> ocGrid;
          MapMetaData mapMetaData;
          void scoreFun(double normal_value,double height,double &score);
          void getMinMaxXY(std::map<Coordiate, int> &tmp_gridPoints,int &min_x,int &max_x,int &min_y,int &max_y);
          void calcSurfaceNormals(PointCloud::Ptr& cloud);
          void updateGrid(PointCloud::Ptr& cloud);
          void getMapMetadata(MapMetaData &data);
          bool isUpdateGrid=false;
     public:
          MyGrid();
          void update(PointCloud::Ptr& cloud,MapMetaData &data);
    };

} 

#endif 
