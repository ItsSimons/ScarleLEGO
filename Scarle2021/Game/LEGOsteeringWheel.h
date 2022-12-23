#pragma once
#include "CustomBaseObject.h"

class LEGOSteeringWheel : public CustomBaseObject
{
public:
    LEGOSteeringWheel(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
        : CustomBaseObject("SteeringWheel", _pd3dDevice, _EF, _physic_scene, _composite_body)
    {
        m_scale = Vector3{ 5.f, 5.f, 5.f };
        base_object_extents	= Vector3{ 9.f, 19.f, 19.f };

        //Turning?
        base_forces[leftward_f] = Vector3(3250, 0, 0);
        base_forces[rightward_f] = Vector3(-3250, 0, 0);
        
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