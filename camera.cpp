// ------------------------------------ //
// ..:: The EnGen Project ::..
//
// File properties:
//   Name:
//     camera.cpp
//   Author:
//     Floris van Riel
//   Description:
//     General methods for the camera position
//     in world space
//
// An AP product (c) 2016
// ------------------------------------ //

#include "camera.h"

Camera::Camera() {
	m_positionX = 0;
	m_positionY = 0;
	m_positionZ = 0;

	m_rotationX = 0;
	m_rotationY = 0;
	m_rotationZ = 0;
}

Camera::Camera(const Camera& other) {
}

Camera::~Camera() {
}

void Camera::SetPosition(float x, float y, float z) {
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;

	return;
}

void Camera::SetRotation(float x, float y, float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}

XMFLOAT3 Camera::GetPosition() {
	return XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}

XMFLOAT3 Camera::GetRotation() {
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render() {
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	// Setup all vectors
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	upVector = XMLoadFloat3(&up);

	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;
	positionVector = XMLoadFloat3(&position);

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	// Setup the rotation matrix using yaw pitch and roll
	pitch = m_rotationX * (XM_PI / 180);
	yaw = m_rotationY * (XM_PI / 180);
	roll = m_rotationZ * (XM_PI / 180);
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Update the up and look at vectors matching the rotation
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	// Adapt the look at vector to the location of the camera
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	// Create the view matrix
	m_viewMatrix = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

	return;
}

XMMATRIX Camera::GetViewMatrix() {
	return m_viewMatrix;
}