
#pragma once
#include "CustomBaseObject.h"

class LEGOwing : public CustomBaseObject
{
public:
    LEGOwing(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
        : CustomBaseObject("WingLeft",_pd3dDevice, _EF, _physic_scene, _composite_body)
    {
        m_scale = Vector3{ 1.f, 1.f, 1.f };
        object_extents = q3Vec3{ 19.0f, 9.0f, 9.0f };
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