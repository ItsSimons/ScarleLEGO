#pragma once
#include "CustomBaseObject.h"
#include "GameData.h"
#include "ObjectList.h"


/**
 * \brief Interface to allow template inheritance.
 */
class ButtonInterface
{
public:
	explicit ButtonInterface(const Vector2& bt_pos, const BlockIndex& block_type, ID3D11Device* _d3dDevice) {};
	virtual ~ButtonInterface() = default;
	
	virtual const BlockIndex& getBlockID() = 0;
	
	virtual void update(GameData* _GD, const Vector2& mouse_pos) = 0;
	virtual void render(DrawData2D* _DD2D) = 0;

	virtual void setPos(const Vector2& new_pos) = 0;
	virtual const Vector2& getPos() = 0;
	virtual const Vector2& getRes() = 0;
};