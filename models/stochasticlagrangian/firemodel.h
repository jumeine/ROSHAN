//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_H
#define ROSHAN_FIREMODEL_H

#include "../../model_interface.h"
#include "firemodel_gridmap.h"
#include "firemodel_renderer.h"
#include "wind.h"


class FireModel : public IModel{
public:
    //only one instance of this class can be created
    static FireModel* GetInstance(SDL_Renderer* renderer) {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModel(renderer);
    }

    void Initialize() override;
    void Update() override;
    void Reset() override;
    void Config() override;
    void Render() override;
    void SetWidthHeight(int width, int height) override;
    void HandleEvents(SDL_Event event, ImGuiIO* io) override;

private:
    FireModel(SDL_Renderer* renderer);
    ~FireModel(){}

    void RandomizeCells();

    GridMap* gridmap_;
    FireModelRenderer* model_renderer_;
    Wind* wind_;
    static FireModel* instance_;
    int cell_size_ = 4;
    double dt_ = 0.1;
    double tau_mem_ = 10;
    int width_;
    int height_;

};


#endif //ROSHAN_FIREMODEL_H
