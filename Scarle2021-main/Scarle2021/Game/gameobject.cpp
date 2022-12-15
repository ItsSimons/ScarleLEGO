#include "pch.h"
#include "gameobject.h"
#include "GameData.h"

GameObject::GameObject()
{
	//set the Gameobject to the origin with no rotation and unit scaling 
	m_pos = Vector3::Zero;
	m_scale = Vector3::One;
	m_pitch = 0.0f;
	m_yaw = 0.0f;
	m_roll = 0.0f;
	
	m_worldMat = Matrix::Identity;
	m_fudge = Matrix::Identity;
}

GameObject::~GameObject()
{

}

void GameObject::Tick(GameData* _GD)
{
	if (m_physicsOn)
	{
		Vector3 newVel = m_vel + _GD->m_dt * (m_acc - m_drag*m_vel);
		Vector3 newPos = m_pos + _GD->m_dt * m_vel;

		m_vel = newVel;
		m_pos = newPos;
	}

	//build up the world matrix depending on the new position of the GameObject
	//the assumption is that this class will be inherited by the class that ACTUALLY changes this
	Matrix scaleMat = Matrix::CreateScale(m_scale);

	switch (m_rotation_method)
	{
	case use_quaternions:
		m_rotMat = Matrix::CreateFromQuaternion(m_rotQuat); //Makes the rotation from a quaternion
		break;

	case use_yaw_pitch_roll:
		m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll); //Makes rotation from yaw, pitch and roll
		break;

	case use_both:
		//Combines both
		//Multiply the two will sum the rotation
		m_rotMat = Matrix::CreateFromYawPitchRoll(m_yaw, m_pitch, m_roll) * Matrix::CreateFromQuaternion(m_rotQuat);
		break;
	}
	
	//Position of the object in the game world
	Matrix transMat = Matrix::CreateTranslation(m_pos);

	//Combines the matrices together to give the object its properties
	m_worldMat = m_fudge * scaleMat * m_rotMat * transMat;

	//zero acceleration ready for the next time round
	m_acc = Vector3::Zero;
}