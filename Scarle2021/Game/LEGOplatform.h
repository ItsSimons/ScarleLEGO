#pragma once
#include "CustomBaseObject.h"

class LEGOplatform : public CustomBaseObject
{
public:
	LEGOplatform(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
		: CustomBaseObject("Platform", _pd3dDevice, _EF, _physic_scene, _composite_body)
	{
		m_scale = Vector3{5.f, 5.f, 5.f};
		object_extents = q3Vec3{101.f, 0.1f, 101.f};
	}
};