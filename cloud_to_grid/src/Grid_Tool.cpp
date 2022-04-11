//
// Created by xia on 20-4-21.
//

#include <cloud_to_grid/Grid_Tool.h>
namespace MyTool {
    void GetGridCoord(const double temp_x,const double temp_y,int &x,int &y) {
        x = int(temp_x / param.cellResolution);
        y = int(temp_y / param.cellResolution);
    }

    void RemoveUnusedPoint(PointCloud::Ptr &cloud) {
#ifdef DEBUG
        cout<<"start RemoveUnusedPoint"<<endl;
#endif
        long i=0;
        PointCloud::iterator it = cloud->points.begin();
        while (it != cloud->points.end()) {
            float x = it->x;
            float y = it->y;
            float z = it->z;
            float rgb = it->rgb;
            if ((z>param.clip_max)||(z<param.clip_min)) {
                it=cloud->points.erase(it);
                i++;
            } else
                ++it;
        }
#ifdef DEBUG
         cout<<"end RemoveUnusedPoint,remove total: "<<i<<endl;
#endif
    }

    MyGrid::MyGrid(void):cloudNormalPtr(new NormalCloud()) {}

    void MyGrid::calcSurfaceNormals(PointCloud::Ptr &cloud) {
#ifdef DEBUG
        cout<<"start calcSurfaceNormals"<<endl;
#endif
        pcl::NormalEstimation<PointType, pcl::Normal> ne;
        ne.setInputCloud(cloud);
        pcl::search::KdTree<PointType>::Ptr tree(new pcl::search::KdTree<PointType>());
        ne.setSearchMethod(tree);
        ne.setRadiusSearch(param.searchRadius);
        cloudNormalPtr->points.clear();
        ne.compute(*cloudNormalPtr);
#ifdef DEBUG
        cout<<"end calcSurfaceNormals"<<endl;
#endif
    }

    void MyGrid::getMinMaxXY(std::map<Coordiate, int> &tmp_gridPoints, int &min_x, int &max_x, int &min_y, int &max_y) {
#ifdef DEBUG
        cout<<"start getMinMaxXY"<<endl;
#endif
        int x, y;
        std::map<Coordiate, int>::iterator it;
        it = tmp_gridPoints.begin();
        while (it != tmp_gridPoints.end()) {
            x = it->first.x;
            y = it->first.y;
            if (x > max_x)
                max_x = x;
            if (x < min_x)
                min_x = x;
            if (y > max_y)
                max_y = y;
            if (y < min_y)
                min_y = y;
            ++it;
        }
#ifdef DEBUG
        cout<<"end getMinMaxXY"<<endl;
#endif
    }

    void MyGrid::updateGrid(PointCloud::Ptr &cloud) {
#ifdef DEBUG
        cout<<"start updateGrid"<<endl;
#endif
        RemoveUnusedPoint(cloud);
        calcSurfaceNormals(cloud);
        for (decltype(cloud->size()) i = 0; i < cloud->size(); i++) {
            double normal_value;
            double height;
            double axis_1;
            double axis_2;
            normal_value = cloudNormalPtr->points[i].normal_z;
            height = cloud->points[i].z;
            axis_1 = cloud->points[i].x;
            axis_2 = cloud->points[i].y;


            double score;
            scoreFun(normal_value, height, score);

            int grid_x, grid_y;
            GetGridCoord(axis_1, axis_2, grid_x, grid_y);
            Coordiate key(grid_x, grid_y);
            int count = gridPoints.count(key);
            if (count > 0) {
                int &gridPointCount = gridPoints.find(key)->second;
                if (score >= param.deviation) {
                    gridPointCount = min(10,gridPointCount+1);
                } else {
                    gridPointCount = max(0, gridPointCount-1);
                }
            } else {
               gridPoints.insert(std::pair<Coordiate, int>(key, 0));
                if (score >= param.deviation)
                    gridPoints[key]++;
            }
        }
#ifdef DEBUG
        cout<<"end updateGrid"<<endl;
#endif
    }

    void MyGrid::scoreFun(double normal_z, double z, double &score) {
        double part_nomal = 1 - fabs(normal_z);
        double part_z = 1 - exp(-fabs(z));
        score = param.sigma * part_nomal + (1.0 - param.sigma) * part_z;
    }

    void MyGrid::getMapMetadata(MapMetaData &data) {
#ifdef DEBUG
         cout<<"start getMapdata"<<endl;
#endif
        isUpdateGrid = true;
        if (gridPoints.size() > 0) {
            int min_x = 0, max_x = 0, min_y = 0, max_y = 0;
            getMinMaxXY(gridPoints, min_x, max_x, min_y, max_y);
            ocGrid = std::vector<signed char>(param.border_grids*param.border_grids,-1);
            auto it = gridPoints.begin();
            while (it != gridPoints.end()) {
                ++it;
                int x = it->first.x;
                int y = it->first.y;
                int position = (y + param.border_grids/2) * param.border_grids + x + param.border_grids/2;
                if(position>ocGrid.size()||position<0)
                    continue;
                int counter = it->second;
                if (counter > param.threshold)
                    ocGrid[position] = 100;
                else ocGrid[position] = 0;

            }
            data.data = ocGrid;
            data.height = param.border_grids;
            data.width = param.border_grids;
            data.xMin = min_x;
            data.yMin = min_y;
            data.map_length = param.map_size;
            data.resolution = param.cellResolution;
        }
#ifdef DEBUG
        cout<<"end getMapdata"<<endl;
#endif
    isUpdateGrid = false;
    }
    void MyGrid::update(PointCloud::Ptr& cloud,MapMetaData &data)
    {
        if(isUpdateGrid ||(!param.mode&&isGlobalUpdate)){return;}
        if(!param.mode){
            gridPoints.clear();
            isGlobalUpdate = true;
        }
        updateGrid(cloud); 
        getMapMetadata(data);
    }
}
