#pragma once
#include "GameData.h"
#include "DrawData.h"
#include "ObjectList.h"

class LoadSaveButton
{
public:
	LoadSaveButton(const Vector2& bt_pos, ID3D11Device* _d3dDevice);
	~LoadSaveButton();

	//scarle
	void update(GameData* _GD, const Vector2& mouse_pos);
	void render(DrawData2D* _DD2D);

	//Getters & setters
	void setPos(const Vector2& new_pos);
	
private:
	bool isInside(const Vector2& point, const Vector2& bt_pos, const Vector2& bt_res);

	enum bt_color
	{
		bt_white,
		bt_red
	};
	
	//Scale and offset
	const Vector2 bt_scale = {4.7f, 1.f};
	const Vector2 offset = {80.f, 0};
	//Load button
	ImageGO2D* load_bg = nullptr;
	TextGO2D* load_text = nullptr;
	Vector2 load_res = {0,0};
	Vector2 load_pos = {0,0};
	bt_color load_current_color = bt_white;
	bt_color load_new_color = bt_white;
	bool inside_load = false;
	bool loaded = false;
	//Save button
	ImageGO2D* save_bg = nullptr;
	TextGO2D* save_text = nullptr;
	Vector2 save_res = {0,0};
	Vector2 save_pos = {0,0};
	bt_color save_current_color = bt_white;
	bt_color save_new_color = bt_white;
	bool inside_save = false;
	bool saved = false;
};
