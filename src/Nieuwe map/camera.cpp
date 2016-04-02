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
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_rotationX = 0;
	m_rotationY = 0;
	m_rotationZ = 0;

	m_lookAt = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
}

Camera::Camera(const Camera& other) {
}

Camera::~Camera() {
}

void Camera::SetPosition(float x, float y, float z) {
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;

	return;
}

void Camera::SetRotation(float x, float y, float z) {
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;

	return;
}

void Camera::Strafe(float d) {
	XMVECTOR distance = XMVectorReplicate(d);
	XMVECTOR right = XMLoadFloat3(&m_right);
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(distance, right, position));
}

void Camera::Walk(float d) {
	XMVECTOR distance = XMVectorReplicate(d);
	XMVECTOR lookAt = XMLoadFloat3(&m_lookAt);
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(distance, lookAt, position));
}

void Camera::Pitch(float angle) {
	XMMATRIX rotation = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));
	XMStoreFloat3(&m_lookAt, XMVector3TransformNormal(XMLoadFloat3(&m_lookAt), rotation));
}

void Camera::RotateY(float angle) {
	XMMATRIX rotation = XMMatrixRotationY(angle);

	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotation));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotation));
	XMStoreFloat3(&m_lookAt, XMVector3TransformNormal(XMLoadFloat3(&m_lookAt), rotation));
}

XMFLOAT3 Camera::GetPosition() {
	return m_position;
}

XMFLOAT3 Camera::GetRotation() {
	return XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}

void Camera::Render() {
	XMVECTOR R = XMLoadFloat3(&m_right);
	XMVECTOR U = XMLoadFloat3(&m_up);
	XMVECTOR L = XMLoadFloat3(&m_lookAt);
	XMVECTOR P = XMLoadFloat3(&m_position);
	XMFLOAT4X4 viewMatrix;

	// Keep camera's axes orthogonal to each other and of unit length.
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));

	// U, L already ortho-normal, so no need to normalize cross product.
	R = XMVector3Cross(U, L);

	// Fill in the view matrix entries.
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&m_right, R);
	XMStoreFloat3(&m_up, U);
	XMStoreFloat3(&m_lookAt, L);

	viewMatrix(0, 0) = m_right.x;
	viewMatrix(1, 0) = m_right.y;
	viewMatrix(2, 0) = m_right.z;
	viewMatrix(3, 0) = x;

	viewMatrix(0, 1) = m_up.x;
	viewMatrix(1, 1) = m_up.y;
	viewMatrix(2, 1) = m_up.z;
	viewMatrix(3, 1) = y;

	viewMatrix(0, 2) = m_lookAt.x;
	viewMatrix(1, 2) = m_lookAt.y;
	viewMatrix(2, 2) = m_lookAt.z;
	viewMatrix(3, 2) = z;

	viewMatrix(0, 3) = 0.0f;
	viewMatrix(1, 3) = 0.0f;
	viewMatrix(2, 3) = 0.0f;
	viewMatrix(3, 3) = 1.0f;

	m_viewMatrix = XMLoadFloat4x4(&viewMatrix);

	return;
}

XMMATRIX Camera::GetViewMatrix() {
	return m_viewMatrix;
}