#include "pch.h"
#include "BlockButton.h"

/**
 * \brief creates a UI block button at the specified position
 * \param bt_pos Position where the UI element will be located
 * \param block_type Block ID of the block that will be created
 * \param _d3dDevice DX11 device
 */
BlockButton::BlockButton(const Vector2& bt_pos, const BlockIndex& block_type, ID3D11Device* _d3dDevice)
	: ButtonInterface(bt_pos, block_type, _d3dDevice), block_id(block_type)
{
	//Inits the background of the button
	button_bg = new ImageGO2D("text_bg", _d3dDevice);
	button_bg->SetOrigin(Vector2(0,0));
	button_bg->SetScale(Vector2(15.f, 1.f));
	//Scaled resolution is found by multiplying scale and base resolution
	button_res = Vector2(button_bg->GetRes().x * button_bg->GetScale().x, button_bg->GetRes().y);
	//To find center point with origin at 0 0 subtracts resolution/2 to the pos 
	button_pos = bt_pos - button_res/2;
	button_bg->SetPos(button_pos);
	
	//name of the button is found via block id
	const std::string bt_name = BlockHelper::GetBlockName(block_id);
	//Inits the button text
	button_text = new TextGO2D(bt_name);
	button_text->SetColour(Color((float*)&Colors::Black));
	button_text->SetPos(button_pos);
}

BlockButton::~BlockButton()
{
	delete button_bg;
	delete button_text;
}

// Scarle --------------------------------------------------------------------------------------------------------------

void BlockButton::update(GameData* _GD, const Vector2& mouse_pos)
{
	//Color is changed if mouse is inside button
	if(isInside(mouse_pos))
	{
		new_color = bt_red;
	}
	else
	{
		new_color = bt_white;
	}

	//If the color has changed, the mouse has moved. Updates data
	if(new_color != current_color)
	{
		switch (new_color)
		{
		case bt_white:
			button_bg->SetColour(Color((float*)&Colors::White));
			block_set = false;
			inside = false;
			break;

		case bt_red:
			button_bg->SetColour(Color((float*)&Colors::OrangeRed));
			inside = true;
			break;
		}
		current_color = new_color;
	}
    
	button_bg->Tick(_GD);
	button_text->Tick(_GD);
}

void BlockButton::render(DrawData2D* _DD2D)
{
	button_bg->Draw(_DD2D);
	button_text->Draw(_DD2D);
}

// Bounds check --------------------------------------------------------------------------------------------------------

/**
 * \brief Checks if a point is inside this current button
 * \param point Vector2 point 
 * \return true if the point is inside 
 */
bool BlockButton::isInside(const Vector2& point) const
{
	//Takes the position and resolution of the button to find the area to check if the point is inside
	if(point.x >= button_pos.x && point.x <= (button_pos.x + button_res.x) &&
	   point.y >= button_pos.y && point.y <= (button_pos.y + button_res.y))
	   	return true;
    
	return false;
}

// Getters & Setters ---------------------------------------------------------------------------------------------------

/**
 * \brief sets a new pos for the UI button 
 * \param new_pos new button location
 */
void BlockButton::setPos(const Vector2& new_pos)
{
	button_pos = new_pos - button_res/2;
	button_bg->SetPos(button_pos);
	button_text->SetPos(button_pos);
}

/**
 * \return Button's position at the centre
 */
const Vector2& BlockButton::getPos()
{
	return button_pos;
}

/**
 * \return button's resolution in pixels
 */
const Vector2& BlockButton::getRes()
{
	return button_res;
}

/**
 * \return Block ID of the button
 */
const BlockIndex BlockButton::getBlockID()
{
	if(inside && !block_set)
	{
		block_set = true;
		return block_id;
	}
	return id_invalid;
}