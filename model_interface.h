//
// Created by nex on 07.06.23.
//

#ifndef ROSHAN_MODEL_INTERFACE_H
#define ROSHAN_MODEL_INTERFACE_H

#include <vector>
#include "imgui.h"
#include <SDL.h>

class IModel {
public:
    virtual ~IModel() = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Config() = 0;
    virtual void Reset() = 0;
    virtual void Render() = 0;
    virtual void SetWidthHeight(int width, int height) = 0;
    virtual void HandleEvents(SDL_Event event, ImGuiIO* io) = 0;
    virtual void ShowPopups() = 0;

};



#endif //ROSHAN_MODEL_INTERFACE_H
