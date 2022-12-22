#pragma once

#include "CMOGO.h"
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

private:
    Vector2 resolution_UI = {1024, 576};


    std::vector<GameObject2D*> elements_UI{};
};

