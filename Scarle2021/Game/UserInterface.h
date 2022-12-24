#pragma once
#include "CustomBaseObject.h"
#include "ObjectList.h"
#include "BlockButton.h"
#include "LoadSaveButton.h"

class UserInterface
{
public:
    UserInterface();
    ~UserInterface();

    void initialize(ID3D11Device* _d3dDevice, const Vector2& resolution);
    const BlockIndex& getSelectionBlockID();
    
    void update(GameData* _GD);
    void render(DrawData2D* _DD2D);

private:
    Vector2 ui_res = {1024, 576};
    Vector2 game_res = {0, 0};

    ImageGO2D* cursor = nullptr;
    Vector2 cursor_scale = {0,0};
    Vector2 cursor_res = {0,0};
    float cursor_speed = 3.f;
    
    std::vector<ButtonInterface*> block_buttons_UI{};
    std::vector<GameObject2D*> elements_UI{};

    LoadSaveButton* test_but = nullptr;
};


