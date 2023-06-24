//
// Created by nex on 24.06.23.
//

#ifndef ROSHAN_DATASET_HANDLER_H
#define ROSHAN_DATASET_HANDLER_H

#include <iostream>
#include <gdal_priv.h>

class DatasetHandler {
public:
    DatasetHandler(std::string path);
//    ~DatasetHandler();

    void ReadDataset();
    void WriteDataset();

    void ShowInfo();

private:
    GDALDataset *dataset_;
};


#endif //ROSHAN_DATASET_HANDLER_H
