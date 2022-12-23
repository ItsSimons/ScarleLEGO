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
	explicit ButtonInterface(const Vector2& bt_pos, ID3D11Device* _d3dDevice) {};
	virtual ~ButtonInterface() = default;

	virtual void setPos(const Vector2& new_pos) = 0;
	
	virtual CustomBaseObject* setBlock(const Vector3& spawn_pos, ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
		q3Scene* _physic_scene, q3Body* _composite_body) = 0;
	
	virtual void update(GameData* _GD, const Vector2& mouse_pos) = 0;
	virtual void render(DrawData2D* _DD2D) = 0;
};