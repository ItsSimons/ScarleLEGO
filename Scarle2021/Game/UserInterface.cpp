#include "pch.h"
#include "UserInterface.h"
#include <iostream>

UserInterface::UserInterface()
= default;

UserInterface::~UserInterface()
{
    delete cursor;
    delete indicator;
    delete indicator_bg;

    for (auto element : elements_UI)
    {
        delete element;
    }
    for (auto button : block_buttons_UI)
    {
        delete button;
    }
    for (auto save_point : save_points_UI)
    {
        delete save_point;
    }
}

/**
 * \brief Inits the game UI
 * \param resolution Game res for UI scaling
 */
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

    //Block indicator
    //Base starting block should be a cube, there should be no downside in hard coding this
    indicator = new TextGO2D("Current: Cube");
    indicator->SetPos(Vector2(0, game_res.y - 64.f));
    indicator->SetColour(Color((float*)&Colors::Black));
    //bg
    indicator_bg = new ImageGO2D("text_bg", _d3dDevice);
    indicator_bg->SetOrigin(Vector2(0,0));
    indicator_bg->SetColour(Color((float*)&Colors::White));
    indicator_bg->SetPos(indicator->GetPos());
    indicator_bg->SetScale(Vector2(17.f, 1.f));
    
    //Iterates through the enums of blocks and creates and UI entry for each one of those, if not blacklisted
    //This to load new elements directly from the block index file
    for (int EnumInt = id_invalid; EnumInt != id_last; EnumInt++)
    {
        //Skips if invalid
        if(EnumInt != id_invalid)
        {
            //blacklists engine related blocks
            if(!(EnumInt == id_LEGOBaseObj || EnumInt == id_LEGOPlatform || EnumInt == id_LEGOStartingCube))
            {
                block_buttons_UI.push_back(new BlockButton(
                    resolution/2, static_cast<BlockIndex>(EnumInt), _d3dDevice));
            }
        }
    }
    
    //Places the buttons in a grid
    for (int i = 0; i < block_buttons_UI.size(); ++i)
    {
        int height_mul = i / 2;
        Vector2 new_pos = Vector2{
            0,static_cast<float>(game_res.y * 0.22 + (block_buttons_UI[i]->getRes().y * 1.15f) * height_mul)};

        //If I is even button will be placed in the column in the left
        if(i % 2 == 0)
        {
            new_pos = Vector2(game_res.x * 0.332f, new_pos.y);
        }
        else
        {
            new_pos = Vector2(game_res.x * 0.668f, new_pos.y);
        }

        block_buttons_UI[i]->setPos(new_pos);
    }

    //inits save points
    //For now they are manyally placed in the UI but they do have a SetPos setter
    save_points_UI.push_back(new LoadSaveButton(
        "..\\SaveData\\slot1.json", Vector2(game_res.x * 0.23, game_res.y * 0.825f), _d3dDevice));
    save_points_UI.push_back(new LoadSaveButton(
        "..\\SaveData\\slot2.json", Vector2(game_res.x * 0.50, game_res.y * 0.825f), _d3dDevice));
    save_points_UI.push_back(new LoadSaveButton(
        "..\\SaveData\\slot3.json", Vector2(game_res.x * 0.77, game_res.y * 0.825f), _d3dDevice)); 
}

void UserInterface::toggleVisibilityUI()
{
    if(!driving)
    {
        //Brings cursor back in the middle
        cursor->SetPos(game_res/2);
        visible = !visible;
    }
}

//Scarle ---------------------------------------------------------------------------------------------------------------

void UserInterface::update(GameData* _GD)
{
    if(!visible) return;
    
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

    //ticks block indicator
    indicator_bg->Tick(_GD);
    indicator->Tick(_GD);
    
    for (const auto element : elements_UI)
    {
        element->Tick(_GD);
    }

    for (const auto button : block_buttons_UI)
    {
        button->update(_GD, mouse_pos);
    }

    for (const auto save_point : save_points_UI)
    {
        save_point->update(_GD, mouse_pos);
    }
}

void UserInterface::render(DrawData2D* _DD2D)
{
    if(!visible)
    {
        if(!driving)
        {
            indicator_bg->Draw(_DD2D);
            indicator->Draw(_DD2D);
        }
        return;
    };
    
    for (const auto element : elements_UI)
    {
        element->Draw(_DD2D);
    }
    for (const auto button : block_buttons_UI)
    {
        button->render(_DD2D);
    }
    for (const auto save_point : save_points_UI)
    {
        save_point->render(_DD2D);
    }
    
    cursor->Draw(_DD2D);
}

// Getters & Setters ---------------------------------------------------------------------------------------------------

/**
 * \brief Updates the visibility of the UI
 */
void UserInterface::setVisibilityUI(bool _visible)
{
    visible = _visible;
}

/**
 * \return is currently visible?
 */
bool UserInterface::getVisibilityUI() const
{
    return visible;
}

/**
 * \brief set driving mode bool
 */
void UserInterface::setDrivingMode(bool _driving)
{
    driving = _driving;
}

/**
 * \brief Checks all the block buttons to see if one has been pressed.
 * \return Returns block ID of pressed button
 */
const BlockIndex& UserInterface::getSelectionBlockID()
{
    if(!visible) return id_invalid;
    
    for (const auto button : block_buttons_UI)
    {
        const BlockIndex return_id = button->getBlockID();
        
        if(return_id != id_invalid)
        {
            //If a selection happened toggle UI
            toggleVisibilityUI();

            //Gets pos and new block name
            const auto indicator_pos = indicator->GetPos();
            const std::string current_block_name = "Current: " + BlockHelper::GetBlockName(return_id); 

            //Deletes old indicator and recreates it with new name
            delete indicator;
            indicator = new TextGO2D(current_block_name);
            indicator->SetPos(indicator_pos);
            indicator->SetColour(Color((float*)&Colors::Black));
            return return_id;
        }
    }
    return id_invalid;
}

/**
 * \brief Checks all the Save buttons to see if one has been pressed.
 * \return Path of file to save to
 */
std::string UserInterface::tryGetSavePath() 
{
    if(!visible) return "null";
    
    for (const auto save_point : save_points_UI)
    {
        std::string path = save_point->getSavePath();

        if(path != "null")
        {
            //Does not close the UI when saving
            std::cout << "Saved!" << std::endl;
            return path;
        }
    }
    return "null";
}

/**
 * \brief Checks all the load buttons to see if one has been pressed.
 * \return Path of file to load from
 */
std::string UserInterface::tryGetLoadPath() 
{
    if(!visible) return "null";
    
    for (const auto save_point : save_points_UI)
    {
        std::string path = save_point->getLoadPath();

        if(path != "null")
        {
            toggleVisibilityUI();
            return path;
        }
    }
    return "null";
}
