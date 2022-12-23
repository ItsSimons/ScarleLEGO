#pragma once
#include "ButtonInterface.h"

/**
 * Templated button class.
 * The type given will be the block returned to the builder
 */
template<class T>
class UIbutton : public ButtonInterface
{
public:
	UIbutton(const Vector2& bt_pos, ID3D11Device* _d3dDevice);
	~UIbutton() override;

	//Returns new building block
	CustomBaseObject* setBlock(const Vector3& spawn_pos, ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
	                           q3Scene* _physic_scene, q3Body* _composite_body) override;

	//Scarle
	void update(GameData* _GD, const Vector2& mouse_pos) override;
	void render(DrawData2D* _DD2D) override;

private:
	//Is point inside button?
	bool isInside(const Vector2& point) const;
	
	ImageGO2D* button_bg = nullptr;
	TextGO2D* button_text = nullptr;

	Vector2 button_res = {0,0};
	Vector2 button_pos = {0,0};
	
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
 * \param _d3dDevice DX11 device
 */
template <class T>
UIbutton<T>::UIbutton(const Vector2& bt_pos, ID3D11Device* _d3dDevice)
	: ButtonInterface(bt_pos, _d3dDevice)
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

	//Inits the button text
	//name of the button is found via class name
	button_text = new TextGO2D(typeid(T).name());
	button_text->SetColour(Color((float*)&Colors::Black));
	button_text->SetPos(button_pos);
}

template <class T>
UIbutton<T>::~UIbutton()
{
	delete button_bg;
	delete button_text;
}

/**
 * \brief Takes a position and generates the corrent building block on the type specified
 * \param spawn_pos Location where the new building block will be located
 * \return pointer to new building block, nullptr if it failed
 */
template <class T>
CustomBaseObject* UIbutton<T>::setBlock(const Vector3& spawn_pos, ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
                                        q3Scene* _physic_scene, q3Body* _composite_body)
{
	//Is cursor inside and block not set yet?
	if(inside && !block_set)
	{
		block_set = true;
		//Creates new building block using template
		CustomBaseObject* new_block = new T(_pd3dDevice, _EF, _physic_scene, _composite_body);
		new_block->SetPos(spawn_pos);
		return new_block;
	}
	return nullptr;
}

template <class T>
void UIbutton<T>::update(GameData* _GD, const Vector2& mouse_pos)
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

template <class T>
void UIbutton<T>::render(DrawData2D* _DD2D)
{
	button_bg->Draw(_DD2D);
	button_text->Draw(_DD2D);
}

/**
 * \brief Is the point inside the button?
 * \param point Vector2 point 
 * \return true if the point is inside 
 */
template <class T>
bool UIbutton<T>::isInside(const Vector2& point) const
{
	if(point.x >= button_pos.x && point.x <= (button_pos.x + button_res.x) &&
	   point.y >= button_pos.y && point.y <= (button_pos.y + button_res.y))
	   	return true;
    
	return false;
}
