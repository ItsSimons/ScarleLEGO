#pragma once
#include "CustomBaseObject.h"

class LEGOwheel : public CustomBaseObject
{
public:
	LEGOwheel(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
		: CustomBaseObject("Wheel", _pd3dDevice, _EF, _physic_scene, _composite_body)
	{
		m_scale = Vector3{ 5.f, 5.f, 5.f };
		base_object_extents	= Vector3{ 9.f, 19.f, 19.f };

		forces_on_touch = true;
		
		base_forces[forward_f] = Vector3(0, 0, 4600);
		base_forces[backward_f] = Vector3(0, 0, -4600);

		//Turning?
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