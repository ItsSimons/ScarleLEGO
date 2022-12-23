#include "pch.h"
#include "iostream"
#include "UserInterface.h"
#include "Handler.h"

UserInterface::UserInterface()
= default;

UserInterface::~UserInterface()
{
    delete cursor;

    for (auto button : buttons_UI)
    {
        delete button;
    }
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

    //UI buttons
    //This is where newly added components are added
    buttons_UI.push_back(new BlockButton<LEGOCube>(resolution/2, _d3dDevice));
    buttons_UI.push_back(new BlockButton<LEGOWheel>(resolution/2, _d3dDevice));
    buttons_UI.push_back(new BlockButton<LEGOSteeringWheel>(resolution/2, _d3dDevice));
    buttons_UI.push_back(new BlockButton<LEGOThruster>(resolution/2, _d3dDevice));
    buttons_UI.push_back(new BlockButton<LEGOWing>(resolution/2, _d3dDevice));

    //Places the buttons in a grid
    for (int i = 0; i < buttons_UI.size(); ++i)
    {
        int height_mul = i / 2;
        Vector2 new_pos = Vector2{
            0,static_cast<float>(game_res.y * 0.22 + (buttons_UI[i]->getRes().y * 1.15f) * height_mul)};

        //If I is even button will be placed in the column in the left
        if(i % 2 == 0)
        {
            new_pos = Vector2(game_res.x * 0.332f, new_pos.y);
        }
        else
        {
            new_pos = Vector2(game_res.x * 0.668f, new_pos.y);
        }

        buttons_UI[i]->setPos(new_pos);
    }
    
}

CustomBaseObject* UserInterface::getSelection(const Vector3& spawn_pos, ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
    q3Scene* _physic_scene, q3Body* _composite_body)
{
    for (auto button : buttons_UI)
    {
        CustomBaseObject* new_block = button->setBlock(spawn_pos, _pd3dDevice, _EF, _physic_scene, _composite_body);

        if(new_block != nullptr)
            return new_block;
    }
    
    return nullptr;
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
        button->update(_GD, mouse_pos);
    }
}

void UserInterface::render(DrawData2D* _DD2D)
{
    for (auto element : elements_UI)
    {
        element->Draw(_DD2D);
    }

    for (auto button : buttons_UI)
    {
        button->render(_DD2D);
    }
    
    cursor->Draw(_DD2D);
}


