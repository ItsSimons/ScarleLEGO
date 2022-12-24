#pragma once
#include "BlockIndex.h"
#include "ButtonInterface.h"

class BlockButton : public ButtonInterface
{
public:
	BlockButton(const Vector2& bt_pos, const BlockIndex& block_type, ID3D11Device* _d3dDevice);
	~BlockButton() override;
	
	//Returns new building block
	const BlockIndex& getBlockID() override;

	//Scarle
	void update(GameData* _GD, const Vector2& mouse_pos) override;
	void render(DrawData2D* _DD2D) override;

	//Getters & setters
	void setPos(const Vector2& new_pos) override;
	const Vector2& getPos() override;
	const Vector2& getRes() override;

private:
	//Is point inside button?
	bool isInside(const Vector2& point) const;
	
	ImageGO2D* button_bg = nullptr;
	TextGO2D* button_text = nullptr;

	Vector2 button_res = {0,0};
	Vector2 button_pos = {0,0};
	BlockIndex block_id;
	
	enum bt_color
	{
		bt_white,
		bt_red
	};
	bt_color current_color = bt_white;
	bt_color new_color = bt_white;
	bool inside = false; //True if point was inside
	bool block_set = false; //True if the new building block has already been returned
	
};

// Inline methods ------------------------------------------------------------------------------------------------------

/**
 * \brief creates a UI block button at the specified position
 * \param bt_pos Position where the UI element will be located
 * \param block_type Block ID of the block that will be created
 * \param _d3dDevice DX11 device
 */
inline BlockButton::BlockButton(const Vector2& bt_pos, const BlockIndex& block_type, ID3D11Device* _d3dDevice)
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
	const std::string bt_name = BlockAssembler::GetBlockName(block_id);
	//Inits the button text
	button_text = new TextGO2D(bt_name);
	button_text->SetColour(Color((float*)&Colors::Black));
	button_text->SetPos(button_pos);
}

inline BlockButton::~BlockButton()
{
	delete button_bg;
	delete button_text;
}

/**
 * \return Block ID of the button
 */
inline const BlockIndex& BlockButton::getBlockID()
{
	if(inside && !block_set)
	{
		block_set = true;
		return block_id;
	}
	return id_invalid;
}

inline void BlockButton::update(GameData* _GD, const Vector2& mouse_pos)
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

inline void BlockButton::render(DrawData2D* _DD2D)
{
	button_bg->Draw(_DD2D);
	button_text->Draw(_DD2D);
}

/**
 * \brief Is the point inside the button?
 * \param point Vector2 point 
 * \return true if the point is inside 
 */
inline bool BlockButton::isInside(const Vector2& point) const
{
	if(point.x >= button_pos.x && point.x <= (button_pos.x + button_res.x) &&
	   point.y >= button_pos.y && point.y <= (button_pos.y + button_res.y))
	   	return true;
    
	return false;
}

/**
 * \brief sets a new pos for the UI button 
 * \param new_pos new button location
 */
inline void BlockButton::setPos(const Vector2& new_pos)
{
	button_pos = new_pos - button_res/2;
	button_bg->SetPos(button_pos);
	button_text->SetPos(button_pos);
}

/**
 * \return Button's position at the centre
 */
inline const Vector2& BlockButton::getPos()
{
	return button_pos;
}

/**
 * \return button's resolution in pixels
 */
inline const Vector2& BlockButton::getRes()
{
	return button_res;
}
