#pragma once
#include "BlockIndex.h"
#include "ButtonInterface.h"

class BlockButton : public ButtonInterface
{
public:
	BlockButton(const Vector2& bt_pos, const BlockIndex& block_type, ID3D11Device* _d3dDevice);
	~BlockButton() override;

	//Scarle
	void update(GameData* _GD, const Vector2& mouse_pos) override;
	void render(DrawData2D* _DD2D) override;

	//Getters & setters
	void setPos(const Vector2& new_pos) override;
	const Vector2& getPos() override;
	const Vector2& getRes() override;
	const BlockIndex& getBlockID(); //Returns new building block id

private:
	//Bounds check
	bool isInside(const Vector2& point) const;
	//Which block will be give from this button?
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

