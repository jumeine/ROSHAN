//
// Created by nex on 24.06.23.
//

#include "dataset_handler.h"

DatasetHandler::DatasetHandler(std::string path) {
    GDALAllRegister();
    dataset_ = (GDALDataset *) GDALOpen(path.c_str(), GA_ReadOnly);
    if (dataset_ == NULL) {
        std::cout << "DatasetHandler: Could not open file: " << path << std::endl;
        exit(1);
    }
    datafilepath_ = "../openstreetmap/data.json";
}

void DatasetHandler::TransformCoordinates(double lng, double lat, double &lng_transformed, double &lat_transformed) {
    OGRSpatialReference oSourceSRS, oTargetSRS;
    OGRCoordinateTransformation *poCT;

    oSourceSRS.importFromEPSG(4326); // Web Mercator projection (OpenStreetMap)
    oTargetSRS.importFromEPSG(3035); // ETRS89 / LAEA Europe

    poCT = OGRCreateCoordinateTransformation(&oSourceSRS, &oTargetSRS);

    if (poCT == NULL) {
        std::cout << "DatasetHandler: Could not create coordinate transformation" << std::endl;
        exit(1);
    }

    double _lat = lat;
    double _lng = lng;

    if (poCT->Transform(1, &_lat, &_lng) != 1) {
        std::cout << "DatasetHandler: Could not transform coordinates" << std::endl;
        exit(1);
    }

    lng_transformed = _lng;
    lat_transformed = _lat;

    OGRCoordinateTransformation::DestroyCT(poCT);
}

void DatasetHandler::GetCoordinatesFromFile() {
    //Open the file
    std::ifstream i(datafilepath_);

    //Parse the file
    json j;
    i >> j;

    // Create an array of pointers to your GeoCoordinates
    coords_[0] = &(rectangle_.lower_left);
    coords_[1] = &(rectangle_.upper_left);
    coords_[2] = &(rectangle_.upper_right);
    coords_[3] = &(rectangle_.lower_right);


    int k = 0;
    for (json& element : j) {
        for (json& coordinate : element) {
            double lat = coordinate["lat"];
            double lng = coordinate["lng"];
            double lat_transformed, lng_transformed;
            TransformCoordinates(lng, lat, lng_transformed, lat_transformed);

            // Check if index is out of bounds
            if (k >= 4) {
                std::cerr << "DatasetHandler: Index out of bounds" << std::endl;
            }

            // Save the transformed coordinates
            coords_[k]->coordinate[0] = lng_transformed;
            coords_[k]->coordinate[1] = lat_transformed;

            k++;
        }
    }
    DeleteDataFile();
}

bool DatasetHandler::NewDataPointExists() {
    return std::filesystem::exists(datafilepath_);
}

void DatasetHandler::DeleteDataFile() {
    try {
        std::filesystem::remove(datafilepath_);
    } catch (std::filesystem::filesystem_error& e) {
        std::cerr << "Error deleting file: " << e.what() << std::endl;
    }
}

void DatasetHandler::SaveRaster() {

    double minX = std::min({rectangle_.lower_left.coordinate[0], rectangle_.lower_right.coordinate[0], rectangle_.upper_left.coordinate[0], rectangle_.upper_right.coordinate[0]});
    double minY = std::min({rectangle_.lower_left.coordinate[1], rectangle_.lower_right.coordinate[1], rectangle_.upper_left.coordinate[1], rectangle_.upper_right.coordinate[1]});
    double maxX = std::max({rectangle_.lower_left.coordinate[0], rectangle_.lower_right.coordinate[0], rectangle_.upper_left.coordinate[0], rectangle_.upper_right.coordinate[0]});
    double maxY = std::max({rectangle_.lower_left.coordinate[1], rectangle_.lower_right.coordinate[1], rectangle_.upper_left.coordinate[1], rectangle_.upper_right.coordinate[1]});

    double adfGeoTransform[6];
    dataset_->GetGeoTransform(adfGeoTransform);
    double pixelWidth = adfGeoTransform[1];
    double pixelHeight = adfGeoTransform[5];

    int startX = (int)((minX - adfGeoTransform[0]) / adfGeoTransform[1]);
    int startY = (int)((maxY - adfGeoTransform[3]) / adfGeoTransform[5]);
    int width  = (int)((maxX - minX) / adfGeoTransform[1]);
    int height = (int)((minY - maxY) / adfGeoTransform[5]);

    // Create a new dataset for the rectangle
    GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
    GDALDataset* poDstDS = poDriver->Create("NewMap.tif", width, height, 1, GDT_Float32, NULL);

    // Set the geotransform and projection on the new dataset
    double adfNewGeoTransform[6] = { minX, pixelWidth, 0, maxY, 0, -pixelHeight };
    poDstDS->SetGeoTransform(adfNewGeoTransform);
    poDstDS->SetProjection(dataset_->GetProjectionRef());

    // Read data from the source dataset into the new dataset
    float* pafScanline = new float[width * height];
    dataset_->GetRasterBand(1)->RasterIO(GF_Read, startX, startY, width, height,
                                         pafScanline, width, height, GDT_Float32,
                                         0, 0);
    poDstDS->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, width, height,
                                        pafScanline, width, height, GDT_Float32,
                                        0, 0);

    // Clean up
    delete[] pafScanline;
    GDALClose(poDstDS);

}

void DatasetHandler::LoadRasterDataFromFile(std::vector<std::vector<int>> &rasterData) {

    GetCoordinatesFromFile();

    double minX = std::min({rectangle_.lower_left.coordinate[0], rectangle_.lower_right.coordinate[0], rectangle_.upper_left.coordinate[0], rectangle_.upper_right.coordinate[0]});
    double minY = std::min({rectangle_.lower_left.coordinate[1], rectangle_.lower_right.coordinate[1], rectangle_.upper_left.coordinate[1], rectangle_.upper_right.coordinate[1]});
    double maxX = std::max({rectangle_.lower_left.coordinate[0], rectangle_.lower_right.coordinate[0], rectangle_.upper_left.coordinate[0], rectangle_.upper_right.coordinate[0]});
    double maxY = std::max({rectangle_.lower_left.coordinate[1], rectangle_.lower_right.coordinate[1], rectangle_.upper_left.coordinate[1], rectangle_.upper_right.coordinate[1]});

    double adfGeoTransform[6];
    dataset_->GetGeoTransform(adfGeoTransform);

    int startX = (int)((minX - adfGeoTransform[0]) / adfGeoTransform[1]);
    int startY = (int)((maxY - adfGeoTransform[3]) / adfGeoTransform[5]);
    int width  = (int)((maxX - minX) / adfGeoTransform[1]);
    int height = (int)((minY - maxY) / adfGeoTransform[5]);

    // Create a 2D vector to store the raster data
    rasterData.resize(width, std::vector<int>(height));

    // Read the data from the input dataset directly into the 2D vector
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int pixelValue;
            dataset_->GetRasterBand(1)->RasterIO(GF_Read, startX + j, startY + i, 1, 1, &pixelValue, 1, 1, GDT_Int32, 0, 0);
            rasterData[j][i] = pixelValue;
        }
    }
}
