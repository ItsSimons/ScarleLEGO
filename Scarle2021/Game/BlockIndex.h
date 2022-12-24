#pragma once
#include "CustomBaseObject.h"
#include "LEGOCube.h"
#include "LEGOPlatform.h"
#include "LEGOStartingBlock.h"
#include "LEGOSteeringWheel.h"
#include "LEGOThruster.h"
#include "LEGOWheel.h"
#include "LEGOWing.h"

/**
 * \brief Index of all the building blocks
 * Add new ones here.
 */
enum BlockIndex
{
    id_invalid,
    id_LEGOBaseObj,
    id_LEGOCube,
    id_LEGOPlatform,
    id_LEGOStartingCube,
    id_LEGOSteeringWheel,
    id_LEGOThruster,
    id_LEGOWheel,
    id_LEGOWing,
    id_last,
};

namespace BlockAssembler
{
    /**
     * \brief Takes in data and a type and return the requested block
     * \param block_pos Position of the block
     * \param block_rot Rotation in Pitch Yaw Roll
     * \param block_type Type of the block to be generated
     * \param _pd3dDevice DX11 device
     * \param _EF Effect factory
     * \param _physic_scene Physic scene
     * \param _composite_body "Vehicle"
     * \return Point to a newly created building block
     */
    static CustomBaseObject* MakeBlock(const Vector3& block_pos, const Vector3& block_rot, const BlockIndex& block_type,
        ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
    {
        CustomBaseObject* new_block = nullptr;

        switch (block_type)
        {
            case id_LEGOBaseObj:
                new_block = new CustomBaseObject(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;
            
            case id_LEGOCube:
                new_block = new LEGOCube(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;

            case id_LEGOPlatform:
                new_block = new LEGOPlatform(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;

            case id_LEGOStartingCube:
                new_block = new LEGOStartingCube(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;

            case id_LEGOSteeringWheel:
                new_block = new LEGOSteeringWheel(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;
        
            case id_LEGOThruster:
                new_block = new LEGOThruster(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;

            case id_LEGOWheel:
                new_block = new LEGOWheel(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;

            case id_LEGOWing:
                new_block = new LEGOWing(_pd3dDevice, _EF, _physic_scene, _composite_body);
                break;
            
            default:
                return nullptr;
        }

        new_block->setID(block_type);
        new_block->SetPos(block_pos);
        new_block->SetPitchYawRoll(block_rot.x, block_rot.y, block_rot.z);
        return new_block;
    }

    /**
     * \return Returns a string which is the name of the block type
     */
    static std::string GetBlockName(const BlockIndex& block_type)
    {
        switch (block_type)
        {
            case id_LEGOBaseObj:
                return "Base Object";

            case id_LEGOCube:
                return "Cube";
                
            case id_LEGOPlatform:
                return "Platform";

            case id_LEGOStartingCube:
                return "Starting Cube";

            case id_LEGOSteeringWheel:
                return "Steering Wheel";

            case id_LEGOThruster:
                return "Thruster";

            case id_LEGOWheel:
                return "Wheel";

            case id_LEGOWing:
                return "Wing";

            default:
                return "invalid id";
        }
    }
}
