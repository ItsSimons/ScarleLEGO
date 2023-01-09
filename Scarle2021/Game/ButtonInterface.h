#pragma once
#include "GameData.h"
#include "ObjectList.h"

/**
 * This was more of a last minute addition, and while it was not necessary
 * it helps hepls working with the UI faster
 */
class ButtonInterface
{
public:
	explicit ButtonInterface() = default;
	virtual ~ButtonInterface() = default;
	
	virtual void update(GameData* _GD, const Vector2& mouse_pos) = 0;
	virtual void render(DrawData2D* _DD2D) = 0;

	virtual void setPos(const Vector2& new_pos) = 0;
	virtual const Vector2& getPos() = 0;
	virtual const Vector2& getRes() = 0;

protected:
	//text and background
	ImageGO2D* button_bg = nullptr;
	TextGO2D* button_text = nullptr;
	//Res and pos
	Vector2 button_res = {0,0};
	Vector2 button_pos = {0,0};
};