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
}
