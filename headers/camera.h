// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     camera.h
//   Author:
//     Floris van Riel
//   Description:
//     Represents the 3D first person camera
//
// An AP product (c) 2016
// ------------------------------------ //

#ifndef _CAMERA_H_
#define _CAMERA_H_

// Includes
#include <DirectXMath.h>
using namespace DirectX;

#ifndef _XM_NO_INTRINSICS_
#define _XM_NO_INTRINSICS_
#endif

class Camera {
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void SetPosition(float, float, float);
	void SetRotation(float, float, float);

	void Strafe(float);
	void Walk(float);

	void Pitch(float);
	void RotateY(float);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetRotation();

	void Render();
	XMMATRIX GetViewMatrix();

private:
	float m_rotationX, m_rotationY, m_rotationZ;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_projMatrix;
	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	XMFLOAT3 m_up;
	XMFLOAT3 m_right;
};

#endif