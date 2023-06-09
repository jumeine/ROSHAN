//
// Created by nex on 08.06.23.
//

#ifndef ROSHAN_FIREMODEL_RENDERER_H
#define ROSHAN_FIREMODEL_RENDERER_H


class FireModelRenderer {
public:
    //only one instance of this class is allowed
    static FireModelRenderer* GetInstance() {
        return instance_ = (instance_ != nullptr) ? instance_ : new FireModelRenderer();
    }


    void Initialize();
    void Update();
    void Config();
    void Reset();
    void Render();
    void SetWidthHeight(int width, int height);

private:
    FireModelRenderer();
    ~FireModelRenderer(){}
    static FireModelRenderer* instance_;
};


#endif //ROSHAN_FIREMODEL_RENDERER_H
