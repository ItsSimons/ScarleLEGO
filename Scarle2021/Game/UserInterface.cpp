#include "pch.h"
#include "iostream"
#include "UserInterface.h"
#include "Handler.h"

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
    //Saves scale and image res
    cursor_scale = cursor->GetScale();
    cursor_res = cursor->GetRes();

    //background
    ImageGO2D* background = new ImageGO2D("background_UI", _d3dDevice);
    background->SetPos(game_res / 2);
    elements_UI.push_back(background);

    //Text?
    //TextGO2D* button = new TextGO2D("Siuum");
    //button->SetPos(Vector2(game_res/2));
    //button->SetColour(Color((float*)&Colors::Yellow));
    //buttons_UI.push_back(button);

    //Ui button?
    test_ui = new UIbutton<LEGOthruster>(resolution/2, _d3dDevice);
}

CustomBaseObject* UserInterface::getSelection(const Vector3& spawn_pos, ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
    q3Scene* _physic_scene, q3Body* _composite_body)
{
    return test_ui->setBlock(spawn_pos, _pd3dDevice, _EF, _physic_scene, _composite_body);
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
    else if(mouse_pos.x > game_res.x - cursor_res.x * cursor_scale.x)
    {
        mouse_pos.x = game_res.x - cursor_res.x * cursor_scale.x;
    }
    if(mouse_pos.y < 0)
    {
        mouse_pos.y = 0;
    }
    else if(mouse_pos.y > game_res.y - cursor_res.y * cursor_scale.y)
    {
        mouse_pos.y = game_res.y - cursor_res.y * cursor_scale.y;
    }
    //Sets pos and updates the cursor
    cursor->SetPos(mouse_pos);
    cursor->Tick(_GD);
    
    for (auto element : elements_UI)
    {
        element->Tick(_GD);
    }

    for (auto button : buttons_UI)
    {
        button->Tick(_GD);
    }

    test_ui->update(_GD, cursor->GetPos());
}

void UserInterface::render(DrawData2D* _DD2D)
{
    for (auto element : elements_UI)
    {
        element->Draw(_DD2D);
    }

    for (auto button : buttons_UI)
    {
        button->Draw(_DD2D);
    }

    test_ui->render(_DD2D);
    
    cursor->Draw(_DD2D);
}


