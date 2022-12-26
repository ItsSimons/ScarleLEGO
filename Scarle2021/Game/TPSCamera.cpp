#include "pch.h"
#include "TPSCamera.h"
#include "GameData.h"

TPSCamera::TPSCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, GameObject* _target, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _up)
{
	m_targetObject = _target;
	m_dpos = _dpos;
}

TPSCamera::~TPSCamera()
{

}

void TPSCamera::Tick(GameData* _GD)
{
	//Creates pointer variables to be passed to the extract function to get yaw pitch roll of the quaternion
	const XMMATRIX* m_rotation_matrix_target = new XMMATRIX(XMMatrixRotationQuaternion(m_targetObject->GetQuaternion()));
	float* new_yaw = new float(0);
	float* new_pitch = new float(0);
	float* new_roll = new float(0);

	//Extracts yaw pitch roll
	ExtractPitchYawRollFromXMMatrix(new_pitch, new_yaw, new_roll, m_rotation_matrix_target);
	
	//Set up position of camera and target position of camera based on new position and orientation of target object
	Matrix rotCam = Matrix::CreateFromYawPitchRoll(*new_yaw, 0.0f, 0.0f);
	
	m_target = m_targetObject->GetPos();
	m_pos = m_target + Vector3::Transform(m_dpos, rotCam) ;

	//and then set up proj and view matrices
	Camera::Tick(_GD);
}

/**
 * \brief Takes in a XMMATRIX and returns yaw pith roll.
 * This had to be made to extract yawpitchroll from a quaternion, as there are no native functions that allow so
 */
void TPSCamera::ExtractPitchYawRollFromXMMatrix(float* flt_p_PitchOut, float* flt_p_YawOut, float* flt_p_RollOut,
	const DirectX::XMMATRIX* XMMatrix_p_Rotation)
{
	//Creates a 4x4 matrix
	DirectX::XMFLOAT4X4 XMFLOAT4X4_Values;

	//Moves data inside the matrix
	DirectX::XMStoreFloat4x4(&XMFLOAT4X4_Values, DirectX::XMMatrixTranspose(*XMMatrix_p_Rotation));

	//mathematical functions to extract yaw pitch roll from the matrix 
	*flt_p_PitchOut = (float)asin(-XMFLOAT4X4_Values._23);
	*flt_p_YawOut = (float)atan2(XMFLOAT4X4_Values._13, XMFLOAT4X4_Values._33);
	*flt_p_RollOut = (float)atan2(XMFLOAT4X4_Values._21, XMFLOAT4X4_Values._22);
}
