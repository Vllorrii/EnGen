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

#define _XM_NO_INTRINSICS_

class Camera {
public:
	Camera();
	Camera(const Camera& other);
	~Camera();

	// Getter and setter methods for the camera position in world space
	XMVECTOR GetPositionXM() const;
	XMFLOAT3 GetPosition() const;
	void SetPosition(float, float, float);
	void SetPosition(const XMFLOAT3&);

	// Getter methods for the camera axis vectors
	XMVECTOR GetRightXM() const;
	XMFLOAT3 GetRight() const;
	XMVECTOR GetUpXM() const;
	XMFLOAT3 GetUp() const;
	XMVECTOR GetLookXM() const;
	XMFLOAT3 GetLook() const;

	// Getter methods for the frustum volume
	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	// Getter methods for the frustum dimensions
	float GetNearWindowWidth() const;
	float GetNearWindowHeight() const;
	float GetFarWindowWidth() const;
	float GetFarWindowHeight() const;

	// Setup the frustum
	void SetLens(float, float, float, float);
	
	// Define the view space with look at parameters
	void LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp);
	void LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	// Getter methods for the view and projection matrices
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;
	XMMATRIX GetViewProjectionMatrix() const;

	// Methods for moving the camera a distance
	void Strafe(float d);
	void Walk(float d);

	// Methods for rotation the camera an angle
	void Pitch(float);
	void RotateY(float);

	// When moving the view matrix should be updated every frame
	void UpdateViewMatrix();

private:
	// Camera parameters relative to world space
	XMFLOAT3 m_position;
	XMFLOAT3 m_right;
	XMFLOAT3 m_up;
	XMFLOAT3 m_look;

	// Frustum properties
	float m_nearZ;
	float m_farZ;
	float m_aspect;
	float m_fovY;
	float m_nearWindowHeight;
	float m_farWindowHeight;

	// View and projection matrices
	XMFLOAT4X4 m_view;
	XMFLOAT4X4 m_proj;
};

#endif