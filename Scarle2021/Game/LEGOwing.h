#pragma once
#include "CustomBaseObject.h"

class LEGOWing : public CustomBaseObject
{
public:
    LEGOWing(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
        : CustomBaseObject("Wing",_pd3dDevice, _EF, _physic_scene, _composite_body)
    {
        m_scale = Vector3{ 5.f, 5.f, 7.5f };
        base_object_extents = Vector3{ 29.0f, 0.5f, 10.5f };

        base_forces[upward_f] = Vector3{0, 2000, 0};
        
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
        //Cheks if a forward force is applied to fly 
        if(input_vector.x != 0)
            CustomBaseObject::applyInputToBlock(_GD, input_vector);
    }
};