#pragma once

#include <q3.h>
#include "CMOGO.h"
#include "CollisionReport.h"

//gameobject with integrated physics
class CustomBaseObject : public CMOGO
{
public:
    //No-model constructor
    CustomBaseObject(ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
        q3Scene* _physic_scene, q3Body* _composite_body);
    //Model constructor, takes in a string which is the filename of the model
    CustomBaseObject(std::string _filename, ID3D11Device* _pd3dDevice, IEffectFactory* _EF,
        q3Scene* _physic_scene, q3Body* _composite_body);
    
    ~CustomBaseObject() override;

    //Rotation
    virtual void yawObject(bool clockwise);
    virtual void pitchObject(bool clockwise);

    //Placing & Removing
    void materialize();
    void deMaterialize();
    bool place();
    
    //Force Handling
    virtual void applyInputToBlock(GameData* _GD, const Vector3& input_vector);
    
    //Scarle
    void Tick(GameData* _GD) override;
    void Draw(DrawData* _DD) override;
    
protected:
    //Rotates all the forces on current object rotation
    void updateDataOnRotation();
    
    //Collisions Check
    bool checkInsideCollisions(q3AABB& AABB_object, CollisionReport* collision_report) const;
    bool checkOutsideCollisions(q3AABB& AABB_object, CollisionReport* collision_report,
                                const q3Vec3& dummy_extents) const;
    
    //Force Handling
    void applyForces(const Vector3& force) const;

    //Data Saving
    void saveData();

    //Variables --------------------------------------------------------------------------------------------------------
    
    //Physic scene and composite body
    q3Scene* physic_scene = nullptr;
    q3Body* composite_body = nullptr;
    
    //Pointer of this specific block in the physic world
    const q3Box* block_self = nullptr;
    //Position offset
    q3Vec3 position_offset = {0,0,0};

    //Object boundaries
    q3Vec3 object_extents;
    Vector3 base_object_extents;
    float extents_offset = 3.f; //How much from the outer faces collisions are checked
    
    bool created = false; //Item has been created?
    bool forces_on_touch = false; //Forces applied only on touch?

    //Forces Data
    enum forces_index
    {
        forward_f,
        backward_f,
        leftward_f,
        rightward_f,
        upward_f,
        downward_f        
    };
    //Forces applied vectors
    std::vector<Vector3> forces{};
    std::vector<Vector3> base_forces{};

    //Forces collision report
    CollisionReport* col_report_forces = nullptr;

    //Saved rotation for input vector
    Quaternion input_rotation;
};

