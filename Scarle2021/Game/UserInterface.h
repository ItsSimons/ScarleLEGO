#pragma once

#include "BlockButton.h"
#include "UIText.h"
#include "LoadSaveButton.h"

class UserInterface
{
public:
    UserInterface();
    ~UserInterface();

    void initialize(ID3D11Device* _d3dDevice, const Vector2& resolution);
    void toggleVisibilityUI();
    
    //Scarle
    void update(GameData* _GD);
    void render(DrawData2D* _DD2D);

    //Getters & setters
    void setPlaceable(bool _placeable, bool _placed) const;
    void setVisibilityUI(bool _visible);
    bool getVisibilityUI() const;
    void setDrivingMode(bool _driving);
    const BlockIndex& getSelectionBlockID();
    std::string tryGetSavePath() const;
    std::string tryGetLoadPath();

private:
    Vector2 game_res = {0, 0};
    bool visible = false;
    bool driving = false;

    //bg
    GameObject2D* background_UI = nullptr;
    //Cursor data 
    ImageGO2D* cursor = nullptr;
    Vector2 cursor_scale = {0,0};
    Vector2 cursor_res = {0,0};
    float cursor_speed = 3.f;
    //Indicator
    UIText* indicator = nullptr;
    //Placeable feedback
    UIText* placing_feedback = nullptr;
    
    std::vector<BlockButton*> block_buttons_UI{};
    std::vector<LoadSaveButton*> save_points_UI{};
};


