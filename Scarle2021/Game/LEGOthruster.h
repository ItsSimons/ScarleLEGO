#pragma once
#include "CustomBaseObject.h"

class LEGOThruster : public CustomBaseObject
{
public:
    LEGOThruster(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene,q3Body* _composite_body)
        : CustomBaseObject("Thruster", _pd3dDevice, _EF, _physic_scene, _composite_body)
    {
        m_scale = Vector3{ 5.f, 5.f, 5.f };
        base_object_extents = Vector3( 9.0f, 9.f, 14.0f );
        
        //Z moves forward/backwords
        //X moves left/right
        //Y vertical movement

        //Applies a force forward on current rotation 
        base_forces[forward_f] = Vector3(0, 0, 3000);
        
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