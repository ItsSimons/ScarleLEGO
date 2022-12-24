#pragma once
#include "CustomBaseObject.h"

class LEGOStartingCube : public CustomBaseObject
{
public:
    LEGOStartingCube(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
        : CustomBaseObject("StartCube", _pd3dDevice, _EF, _physic_scene, _composite_body)
    {
        m_scale = Vector3{ 5.f, 5.f, 5.f };
        base_object_extents = Vector3{ 9.0f, 9.0f, 9.0f };

        saveData();
    }
};
