#include "pch.h"
#include "iostream"
#include "UserInterface.h"

#include "LEGOplatform.h"

UserInterface::UserInterface()
= default;

UserInterface::~UserInterface()
{
    delete cursor;
    
    for (auto element : elements_UI)
    {
        delete element;
    }
}

void UserInterface::initialize(ID3D11Device* _d3dDevice, const Vector2& resolution)
{
    game_res = resolution;

    //Creates a mouse cursor and places it at the center
    cursor = new ImageGO2D("cursor", _d3dDevice);
    cursor->SetOrigin(Vector2(0,0));
    cursor->SetScale(Vector2(0.08f, 0.08f));
    cursor->SetPos(game_res / 2);

    //background
    ImageGO2D* background = new ImageGO2D("background_UI", _d3dDevice);
    background->SetPos(game_res / 2);
    elements_UI.push_back(background);

}

void UserInterface::update(GameData* _GD)
{
    //Updates the cursor pos with the mouse offset
    const auto& cursor_pos = cursor->GetPos();
    Vector2 mouse_pos = Vector2(cursor_pos.x + _GD->m_MS.x * cursor_speed,
                                cursor_pos.y + _GD->m_MS.y * cursor_speed);

    //Prevents cursor from going outside the window
    if(mouse_pos.x < 0)
    {
        mouse_pos.x = 0;
    }
    else if(mouse_pos.x > game_res.x - 392*0.08)
    {
        mouse_pos.x = game_res.x - 392*0.08;
    }
    if(mouse_pos.y < 0)
    {
        mouse_pos.y = 0;
    }
    else if(mouse_pos.y > game_res.y - 512*0.08)
    {
        mouse_pos.y = game_res.y - 512*0.08;
    }
    //Sets pos and updates the cursor
    cursor->SetPos(mouse_pos);
    cursor->Tick(_GD);
    
    for (auto element : elements_UI)
    {
        element->Tick(_GD);
    }
}

void UserInterface::render(DrawData2D* _DD2D)
{
    for (auto element : elements_UI)
    {
        element->Draw(_DD2D);
    }

    cursor->Draw(_DD2D);
}


//CustomBaseObject* UserInterface::testone()
//{
//    LEGOplatform* hello = new LEGOplatform()
//    return hello;
//}
