#pragma once

#pragma once
#include "CustomBaseObject.h"

class LEGOwheel : public CustomBaseObject
{
public:
	LEGOwheel(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
		: CustomBaseObject("wheelLeft", _pd3dDevice, _EF, _physic_scene, _composite_body)
	{
		m_scale = Vector3{ 13.f, 10.f, 10.f };
		object_extents = q3Vec3{ 8.0f, 19.0f, 19.0f };

		base_forces[forward_f] = Vector3(0, 0, 1500);
		base_forces[backward_f] = Vector3(0, 0, -1500);

		//base_forces[leftward_f] = Vector3(2250, 0, 0);
		//base_forces[rightward_f] = Vector3(-2250, 0, 0);
		
		saveData();
	}

	void yawObject(bool clockwise) override
	{
		CustomBaseObject::yawObject(clockwise);		
	}

	void pitchObject(bool clockwise) override
	{
		CustomBaseObject::pitchObject(clockwise);
	}

	void applyInputToBlock(GameData* _GD, const Vector3& input_vector) override
	{
		CustomBaseObject::applyInputToBlock(_GD, input_vector);
	}
};