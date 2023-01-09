#include "pch.h"
#include "UIText.h"

UIText::UIText(const std::string _string, const Vector2& bt_pos, ID3D11Device* _d3dDevice)
{
    //text
    button_text = new TextGO2D(_string);
    button_text->SetPos(bt_pos);
    //defaults to black
    current_text_color = (float*)&Colors::Black;
    button_text->SetColour(Color(current_text_color));
    //bg
    button_bg = new ImageGO2D("text_bg", _d3dDevice);
    button_bg->SetOrigin(Vector2(0,0));
    //defaults to white
    current_bg_color = (float*)&Colors::White;
    button_bg->SetColour(Color(current_bg_color));
    button_bg->SetPos(bt_pos);
    button_bg->SetScale(Vector2(17.f, 1.f));
    //saves pos
    button_pos = bt_pos;
}

UIText::~UIText()
{
    //Those can't be deleted as they just hold a pointer.
    //Crash if deleted
    current_bg_color = nullptr;
    current_text_color = nullptr;
    
    delete button_bg;
	delete button_text;
}

//Scarle ---------------------------------------------------------------------------------------------------------------

void UIText::update(GameData* _GD, const Vector2& mouse_pos)
{
    button_bg->Tick(_GD);
    button_text->Tick(_GD);
}

void UIText::render(DrawData2D* _DD2D)
{
    button_bg->Draw(_DD2D);
    button_text->Draw(_DD2D);
}

//Getters & Setters ----------------------------------------------------------------------------------------------------

/**
 * \brief Changes the text displayed 
 * \param _string new text
 */
void UIText::setText(const std::string _string)
{
    //To change the string recreates the TextGO and sets back its properties
    button_text = new TextGO2D(_string);
    button_text->SetPos(button_pos);
    button_text->SetColour(Color(current_text_color));
}

/**
 * \brief Takes a new color type punned as a float for the Bg
 * \param color float value of color
 */
void UIText::setBgColor(float* color)
{
    current_bg_color = color;
    button_bg->SetColour(Color(current_bg_color));
}

/**
 * \brief Takes a new color type punned as a float for the Text
 * \param color float value of color
 */
void UIText::setTextColor(float* color)
{
    current_text_color = color;
    button_text->SetColour(Color(current_bg_color));
}

/**
 * \brief sets a new pos for the UI button 
 * \param new_pos new button location
 */
void UIText::setPos(const Vector2& new_pos)
{
    button_pos = new_pos - button_res/2;
    button_bg->SetPos(button_pos);
    button_text->SetPos(button_pos);
}

/**
 * \return Button's position at the centre
 */
const Vector2& UIText::getPos()
{
    return button_pos;
}

/**
 * \return button's resolution in pixels
 */
const Vector2& UIText::getRes()
{
    return button_res;
}
