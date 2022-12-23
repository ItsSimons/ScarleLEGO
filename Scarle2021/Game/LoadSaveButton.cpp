#include "pch.h"
#include "LoadSaveButton.h"

#include <iostream>

LoadSaveButton::LoadSaveButton(const Vector2& bt_pos, ID3D11Device* _d3dDevice)
{
    load_bg = new ImageGO2D("text_bg", _d3dDevice);
    load_bg->SetOrigin(Vector2(0,0));
    load_bg->SetScale(bt_scale);

    load_res = Vector2(load_bg->GetRes().x * bt_scale.x, load_bg->GetRes().y);
    load_pos = bt_pos - load_res/2 + offset;

    load_bg->SetPos(load_pos);

    load_text = new TextGO2D(" Load");
    load_text->SetColour(Color((float*)&Colors::Black));
    load_text->SetPos(load_pos);


    
    save_bg = new ImageGO2D("text_bg", _d3dDevice);
    save_bg->SetOrigin(Vector2(0,0));
    save_bg->SetScale(bt_scale);

    save_res = Vector2(save_bg->GetRes().x * bt_scale.x, save_bg->GetRes().y);
    save_pos = bt_pos - save_res/2 - offset;

    save_bg->SetPos(save_pos);

    save_text = new TextGO2D(" Save");
    save_text->SetColour(Color((float*)&Colors::Black));
    save_text->SetPos(save_pos);
}

LoadSaveButton::~LoadSaveButton()
{
    delete load_bg;
    delete load_text;
    delete save_bg;
    delete save_text;
}

void LoadSaveButton::update(GameData* _GD, const Vector2& mouse_pos)
{
    //Load button highlighting
    if(isInside(mouse_pos, load_pos, load_res))
    {
        load_new_color = bt_red;
    }
    else
    {
        load_new_color = bt_white;
    }

    if(load_new_color != load_current_color)
    {
        switch (load_new_color)
        {
        case bt_white:
            load_bg->SetColour(Color((float*)&Colors::White));
            inside_load = false;
            loaded = false;
            break;
            
        case bt_red:
            load_bg->SetColour(Color((float*)&Colors::OrangeRed));
            inside_load = true;
            break;
        }
        load_current_color = load_new_color;
    }
    //Save button highlighting
    if(isInside(mouse_pos, save_pos, save_res))
    {
        save_new_color = bt_red;
    }
    else
    {
        save_new_color = bt_white;
    }
    
    if(save_new_color != save_current_color)
    {
        switch (save_new_color)
        {
        case bt_white:
            save_bg->SetColour(Color((float*)&Colors::White));
            inside_save = false;
            saved = false;
            break;
            
        case bt_red:
            save_bg->SetColour(Color((float*)&Colors::OrangeRed));
            inside_save = true;
            break;
        }
        save_current_color = save_new_color;
    }
    
    load_bg->Tick(_GD);
    load_text->Tick(_GD);
    save_bg->Tick(_GD);
    save_text->Tick(_GD);
}

void LoadSaveButton::render(DrawData2D* _DD2D)
{
    load_bg->Draw(_DD2D);
    load_text->Draw(_DD2D);
    save_bg->Draw(_DD2D);
    save_text->Draw(_DD2D);
}

bool LoadSaveButton::isInside(const Vector2& point, const Vector2& bt_pos, const Vector2& bt_res)
{
    if(point.x >= bt_pos.x && point.x <= (bt_pos.x + bt_res.x) &&
       point.y >= bt_pos.y && point.y <= (bt_pos.y + bt_res.y))
       return true;
    
    return false;
}

void LoadSaveButton::setPos(const Vector2& new_pos)
{
    load_pos = new_pos - load_res/2 + offset;
    load_bg->SetPos(load_pos);
    load_text->SetPos(load_pos);
    
    save_pos = new_pos - save_res/2 - offset;
    save_bg->SetPos(save_pos);
    save_text->SetPos(save_pos);
}



