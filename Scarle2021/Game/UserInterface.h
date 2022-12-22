#pragma once

#include "CMOGO.h"
#include "CustomBaseObject.h"
#include "GameData.h"
#include "DrawData2D.h"
#include "ObjectList.h"

class UserInterface
{
public:
    UserInterface();
    ~UserInterface();

    void initialize(ID3D11Device* _d3dDevice, const Vector2& resolution);
    void update(GameData* _GD);
    void render(DrawData2D* _DD2D);

    CustomBaseObject* testone();

private:
    Vector2 ui_res = {1024, 576};
    Vector2 game_res = {0, 0};

    GameObject2D* cursor = nullptr;
    float cursor_speed = 3.f;
    
    std::vector<GameObject2D*> elements_UI;
};


