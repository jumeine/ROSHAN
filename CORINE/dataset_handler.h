//
// Created by nex on 24.06.23.
//

#ifndef ROSHAN_DATASET_HANDLER_H
#define ROSHAN_DATASET_HANDLER_H

#include <iostream>
#include <gdal_priv.h>
#include "../nlohmann/json.hpp"
#include <fstream>

using json = nlohmann::json;

struct GeoCoordinate {
    double coordinate[2];
};

struct GeoRectangle {
    GeoCoordinate upper_left;
    GeoCoordinate upper_right;
    GeoCoordinate lower_left;
    GeoCoordinate lower_right;
};

class DatasetHandler {
public:
    DatasetHandler(std::string path);
//    ~DatasetHandler();

    bool NewDataPointExists();
    void LoadRasterDataFromFile(std::vector<std::vector<int>> &rasterData);
    void SaveRaster();

    void ShowInfo();

private:
    GDALDataset *dataset_;
    std::string datafilepath_;
    GeoRectangle rectangle_;
    GeoCoordinate* coords_[4];
    void TransformCoordinates(double lng, double lat, double &lng_transformed, double &lat_transformed);
    void GetCoordinatesFromFile();
    void DeleteDataFile();
};


#endif //ROSHAN_DATASET_HANDLER_H
