//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_H
#define ROSHAN_FIREMODEL_H

#include <set>
#include <map>
#include "../../model_interface.h"
#include "firemodel_gridmap.h"
#include "firemodel_renderer.h"
#include "model_parameters.h"
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
    void ShowPopups() override;

private:
    FireModel(SDL_Renderer* renderer);
    ~FireModel(){}

    void RandomizeCells();


    GridMap* gridmap_;
    FireModelRenderer* model_renderer_;
    Wind* wind_;
    FireModelParameters parameters_;
    static FireModel* instance_;
    double running_time_;

    //For the Popup of Cells
    bool show_popup_ = false;
    std::set<std::pair<int, int>> popups_;
    std::map<std::pair<int, int>, bool> popup_has_been_opened_;

};


#endif //ROSHAN_FIREMODEL_H
