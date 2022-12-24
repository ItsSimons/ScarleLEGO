#pragma once
#include "CustomBaseObject.h"

class LEGOPlatform : public CustomBaseObject
{
public:
	LEGOPlatform(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
		: CustomBaseObject("Platform", _pd3dDevice, _EF, _physic_scene, _composite_body)
	{
		m_scale = Vector3{5.f, 5.f, 5.f};
		base_object_extents = Vector3{101.f, 0.1f, 101.f};

		saveData();
	}
};