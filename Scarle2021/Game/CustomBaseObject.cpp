#include "pch.h"
#include "iostream"
#include "CustomBaseObject.h"
#include <corecrt_math_defines.h>
#include "CollisionReport.h"

/**
 * \brief 
 * \param _pd3dDevice 3D11 device
 * \param _EF Effect Factory
 * \param _physic_scene Current Physic Scene 
 * \param _composite_body "vehicle"
 */
CustomBaseObject::CustomBaseObject(ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene* _physic_scene, q3Body* _composite_body)
    : CMOGO("cubetto", _pd3dDevice, _EF), physic_scene(_physic_scene), composite_body(_composite_body)
{
    //Right scaling and extents for cubetto, default if model is not specified
    m_scale = Vector3{0.1f, 0.05f, 0.05f};
    object_extents = q3Vec3{ 9.0f, 9.0f, 9.0f};
}

/**
 * \brief 
 * \param _filename Model name
 * \param _pd3dDevice 3D11 device
 * \param _EF Effect Factory
 * \param _physic_scene Current Physic Scene
 * \param _composite_body "vehicle"
 */
CustomBaseObject::CustomBaseObject(std::string _filename, ID3D11Device* _pd3dDevice, IEffectFactory* _EF, q3Scene*
    _physic_scene, q3Body* _composite_body) : CMOGO(_filename, _pd3dDevice, _EF),
    physic_scene(_physic_scene), composite_body(_composite_body)
{
    //Set deafult scale
    m_scale = Vector3::One;
    object_extents =  q3Vec3{1.f,1.f,1.f};

    //Forces vectors, applied ones and base ones
    forces.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        forces.push_back(Vector3(Vector3::Zero));
    }
    base_forces.reserve(6);
    for (int i = 0; i < 6; ++i)
    {
        base_forces.push_back(Vector3(Vector3::Zero));
    }
}

CustomBaseObject::~CustomBaseObject()
{
    //When the object gets deleted, delete the hitbox with it
    if (created)
    {
        deMaterialize();
    }
    
    //Sets all the basic pointers to nullptr
    physic_scene = nullptr;
    composite_body = nullptr;
    block_self = nullptr;
}

// Rotation ------------------------------------------------------------------------------------------------------------

/**
 * Rotates an object 90 degrees on its yaw axis
 */
void CustomBaseObject::yawObject(bool clockwise)
{
    //yaw object 90° clockwise or counterclockwise
    clockwise ? m_yaw = m_yaw - M_PI/2 : m_yaw = m_yaw + M_PI/2;
    //Afdter a full turn bring back to 0
    m_yaw >= M_PI*2 || m_yaw <= -M_PI*2 ? m_yaw = 0 : m_yaw;

    //Updates data with rotation
    updateDataOnRotation();
}

/**
 * Rotates an object 90 degrees on its pitch axis
 */
void CustomBaseObject::pitchObject(bool clockwise)
{
    //pitch object 90° clockwise or counterclockwise
    clockwise ? m_pitch = m_pitch - M_PI/2 : m_pitch = m_pitch + M_PI/2;
    //Afdter a full turn bring back to 0
    m_pitch >= M_PI*2 || m_pitch <= -M_PI*2 ? m_pitch = 0 : m_pitch;

    //Updates data with rotation
    updateDataOnRotation();
}

/**
 * Updates vector and forces based on current block's rotation
 * This is so that input and behaviour is consistent after the block is rotated
 */
void CustomBaseObject::updateDataOnRotation()
{
    //Gets the object's current rotation from pitch yaw and roll, they are different as this is made to be compatible
    //with the physics library.
    const Quaternion block_rot = XMQuaternionRotationRollPitchYawFromVector(Vector3(m_pitch, m_yaw, m_roll));
    //Saves a different rotation for rotating the input vector
    input_rotation = XMQuaternionRotationRollPitchYawFromVector(Vector3(m_roll, m_pitch, m_yaw));
    
    //Adaps the base extents to the new rotation 
    const Vector3 extents_rot = XMVector3Rotate(base_object_extents, block_rot);
    //Rounds result and turns into a q3vector
    const q3Vec3 extents_rot_q3 = q3Vec3(round(extents_rot.x), round(extents_rot.y), round(extents_rot.z));
    //Absolutes the values and applies the new extents
    object_extents = q3Vec3(abs(extents_rot_q3.x), abs(extents_rot_q3.y), abs(extents_rot_q3.z));
    
    //Adapts each force to the new block rotation
    for (int i = 0; i < forces.size(); ++i)
    {
        Vector3& force = forces[i];

        //Rotates the force on current object rotation
        force = XMVector3Rotate(base_forces[i], block_rot);

        //Rounds values
        force.x = round(force.x);
        force.y = round(force.y);
        force.z = round(force.z);
    }
}

// Placing & Removing --------------------------------------------------------------------------------------------------

/**
 * Materialises the now virtual object into the physic world
 */
void CustomBaseObject::materialize()
{
    q3BoxDef hitbox_obj;
    q3Transform transform_obj;
    q3Identity(transform_obj);

    //From the existing position of the cube gives it a hitbox and physics
    position_offset = {m_pos.x, m_pos.y, m_pos.z};
    transform_obj.position = position_offset;

    //Sizes the hitbox 
    const q3Vec3 extents_obj = {object_extents};
    hitbox_obj.Set(transform_obj, extents_obj);
    
    //Adds the box to the physics engine and
    //Saves the memory location of the specific body
    block_self = composite_body->AddBox(hitbox_obj);
    
    m_rotation_method = use_both;
    created = true;
}

/**
 * Removes the object from the physic world
 */
void CustomBaseObject::deMaterialize()
{
    //Removes the body form the physics world
    if(created)
    {
        composite_body->RemoveBox(block_self);
        m_rotation_method = use_yaw_pitch_roll;
        created = false;
    }
}

/**
 * Checks collision and proximity to other blocks to determine if a block is placeable
 * If it is, materialises the block in place
 */
bool CustomBaseObject::place()
{
    //Materialises the item 
    materialize();

    //Creates an AABB object
    q3AABB AABB_object;
    //Creates a new collision report object to work with the query function
    auto* collision_report = new CollisionReport;
    //Creates a vector to check the surroundings
    const q3Vec3 dummy_extents_x =
        {object_extents.x + extents_offset, object_extents.y, object_extents.z};
    const q3Vec3 dummy_extents_y =
        {object_extents.x, object_extents.y + extents_offset, object_extents.z};
    const q3Vec3 dummy_extents_z =
        {object_extents.x, object_extents.y, object_extents.z + extents_offset};

    //check inside and outside collisions
    if(checkInsideCollisions(AABB_object, collision_report))
    {
        if (checkOutsideCollisions(AABB_object, collision_report, dummy_extents_x) ||
            checkOutsideCollisions(AABB_object, collision_report, dummy_extents_y) ||
            checkOutsideCollisions(AABB_object, collision_report, dummy_extents_z))
        {
            std::cout << "object placed!" << std::endl;
            delete collision_report;
            return true;
        }
    }

    //If the object is not placeable in the physic world delete it
    deMaterialize();
    delete collision_report;
    return false;
}

// Collision Check -----------------------------------------------------------------------------------------------------

/**
 * Checks for collisions with the current object in the physics world
 */
bool CustomBaseObject::checkInsideCollisions(q3AABB& AABB_object, CollisionReport* collision_report) const
{
    //Find AABB of the object that needs to be placed
    block_self->ComputeAABB(composite_body->GetTransform(), &AABB_object);
    //Sets the colliding object count to 0
    collision_report->i = 0;
    //Searches the physic scene for collisions
    physic_scene->QueryAABB(collision_report, AABB_object);

    //The first collision is not taken into account as it is with itself
    if(collision_report->i > 1)
    {
        //More than one?
        return false;
    }
    return true;
}

/**
 * Checks collisions in a small area on each of the face of the block.
 * The main use for this is sorting out if a block is facing another one in order to be placed
 */
bool CustomBaseObject::checkOutsideCollisions(q3AABB& AABB_object, CollisionReport* collision_report,
                                              const q3Vec3& dummy_extents) const
{
    //Creates a dummy physic box
    q3BoxDef dummy_hitbox;
    q3Transform dummy_transform;
    q3Identity(dummy_transform);

    //Puts it in the same position of the actual hit box of the object 
    dummy_transform.position = position_offset;
    //Extends the extents slightly
    dummy_hitbox.Set(dummy_transform, dummy_extents);

    //Adds the dummy box to the physic world
    const q3Box* dummy_box = composite_body->AddBox(dummy_hitbox);
    //and gets its AABB 
    dummy_box->ComputeAABB(composite_body->GetTransform(), &AABB_object);

    //resets collision report
    collision_report->i = 0;
    //Queries the scene for collision with this object
    physic_scene->QueryAABB(collision_report, AABB_object);
    //then deletes it
    composite_body->RemoveBox(dummy_box);
    dummy_box = nullptr;

    //For each collision greater than 2 means that we have a object that can be snapped to nearby
    if(collision_report->i <= 2)
    {
        return false;
    }
    return true;
}

// Force handling ------------------------------------------------------------------------------------------------------

/**
 * Taking a input vector, calls the correct branch to apply forces
 */
void CustomBaseObject::applyInputToBlock(GameData* _GD, const Vector3& input_vector)
{
    //Does not apply a force if no input has been received
    if(input_vector == Vector3::Zero) return;

    //Rotates vector based on current block rotation, not vehicle rotation
    Vector3 input_vector_rot = XMVector3Rotate(input_vector, input_rotation);
    //Rounding input vec to 1
    input_vector_rot.x = round(input_vector_rot.x);
    input_vector_rot.y = round(input_vector_rot.y);
    input_vector_rot.z = round(input_vector_rot.z);
    
    //W & S
    if(input_vector_rot.x > 0)
    {
        applyForces(forces[backward_f]);
    }
    else if(input_vector_rot.x < 0)
    {
        applyForces(forces[forward_f]);
    }
    //A & D
    if(input_vector_rot.y > 0)
    {
        applyForces(forces[leftward_f]);
    }
    else if(input_vector_rot.y < 0)
    {
        applyForces(forces[rightward_f]);
    }
    //SPACE & CTRL
    if(input_vector_rot.z > 0)
    {
        applyForces(forces[upward_f]);
    }
    else if(input_vector_rot.z < 0)
    {
        applyForces(forces[downward_f]);
    }
}

/**
 * Based on current object rotation, applies forces accordingly
 */
void CustomBaseObject::applyForces(const Vector3& force) const
{
    //No reason to apply a null force 
    if(force == Vector3::Zero) return;
    
    //Gets the current rotation of the block and converts it in DX quaternions
    const auto& block_rot = block_self->body->GetQuaternion();
    const auto& block_rot_DX = Quaternion(block_rot.x, block_rot.y, block_rot.z, block_rot.w);
    
    //Takes the force that should be added and rotates its vector accordingly
    const Vector3 force_rot_DX = XMVector3Rotate(force, block_rot_DX);
    const auto force_rotated = q3Vec3(force_rot_DX.x, force_rot_DX.y, force_rot_DX.z);
        
    //Gets the position of where the force should be applied
    const auto force_pos = q3Vec3(m_pos.x, m_pos.y, m_pos.z);

    //Applies force
    block_self->body->ApplyLinearImpulseAtWorldPoint(force_rotated, force_pos);
}

// Data Saving ---------------------------------------------------------------------------------------------------------

/**
 * Saves basic data for use later on
 */
void CustomBaseObject::saveData()
{
    //Updates the object extents 
    object_extents = q3Vec3(base_object_extents.x, base_object_extents.y, base_object_extents.z);

    //Applies the base force to the force used in movement
    for (int i = 0; i < forces.size(); ++i)
    {
        Vector3& force = forces[i];
        const Vector3& base_force = base_forces[i];

        force = base_force;
    }
}

// Scarle --------------------------------------------------------------------------------------------------------------

void CustomBaseObject::Tick(GameData* _GD)
{
    if(created)
    {
        //Gets position and rotation from the physic engine
        auto obj_pos = block_self->body->GetWorldPoint(position_offset);
        const auto& obj_rot= block_self->body->GetQuaternion();
        
        //applies them accordingly
        m_pos = Vector3(obj_pos.x, obj_pos.y, obj_pos.z);
        m_rotQuat = Quaternion(obj_rot.x, obj_rot.y, obj_rot.z, obj_rot.w);
    }
    CMOGO::Tick(_GD);
}

void CustomBaseObject::Draw(DrawData* _DD)
{
    CMOGO::Draw(_DD);
}

