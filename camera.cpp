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
	m_right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_look = XMFLOAT3(0.0f, 0.0f, 1.0f);

	SetLens(0.25f * XM_PI, 1.0f, 1.0f, 1000.0f);

	m_nearZ = 0;
	m_farZ = 0;
	m_aspect = 0;
	m_fovY = 0;
	m_nearWindowHeight = 0;
	m_farWindowHeight = 0;

	XMStoreFloat4x4(&m_view, XMMatrixIdentity());
	XMStoreFloat4x4(&m_proj, XMMatrixIdentity());
}

Camera::Camera(const Camera& other) {
}

Camera::~Camera() {
}

XMVECTOR Camera::GetPositionXM() const {
	return XMLoadFloat3(&m_position);
}

XMFLOAT3 Camera::GetPosition() const {
	return m_position;
}

void Camera::SetPosition(float x, float y, float z) {
	m_position = XMFLOAT3(x, y, z);
}

void Camera::SetPosition(const XMFLOAT3& v) {
	m_position = v;
}

XMVECTOR Camera::GetRightXM() const {
	return XMLoadFloat3(&m_right);
}

XMFLOAT3 Camera::GetRight() const {
	return m_right;
}

XMVECTOR Camera::GetUpXM() const {
	return XMLoadFloat3(&m_up);
}

XMFLOAT3 Camera::GetUp() const {
	return m_up;
}

XMVECTOR Camera::GetLookXM() const {
	return XMLoadFloat3(&m_look);
}

XMFLOAT3 Camera::GetLook() const {
	return m_look;
}

float Camera::GetNearZ() const {
	return m_nearZ;
}

float Camera::GetFarZ() const {
	return m_farZ;
}

float Camera::GetAspect() const {
	return m_aspect;
}

float Camera::GetFovY() const {
	return m_fovY;
}

float Camera::GetFovX() const {
	float halfWidth = 0.5f * GetNearWindowHeight();
	return 2.0f * atanf(halfWidth / m_nearZ);
}

float Camera::GetNearWindowWidth() const {
	return m_nearWindowHeight * m_aspect;
}

float Camera::GetNearWindowHeight() const {
	return m_nearWindowHeight;
}

float Camera::GetFarWindowWidth() const {
	return m_farWindowHeight * m_aspect;
}

float Camera::GetFarWindowHeight() const {
	return m_farWindowHeight;
}

void Camera::SetLens(float fovY, float aspect, float zn, float zf) {
	m_fovY = fovY;
	m_aspect = aspect;
	m_nearZ = zn;
	m_farZ = zf;

	// Calculate the height af the near and far plane of the frustum
	m_nearWindowHeight = 2.0f * m_nearZ * tanf(0.5f * fovY);
	m_farWindowHeight = 2.0f * m_farZ * tanf(0.5f * fovY);

	// Create the projection matrix
	XMMATRIX p = XMMatrixPerspectiveFovLH(m_fovY, m_aspect, m_nearZ, m_farZ);
	XMStoreFloat4x4(&m_proj, p);

	return;
}

void Camera::LookAt(FXMVECTOR pos, FXMVECTOR target, FXMVECTOR worldUp) {
	XMVECTOR look = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(worldUp, look));
	XMVECTOR up = XMVector3Cross(look, right);

	XMStoreFloat3(&m_position, pos);
	XMStoreFloat3(&m_look, look);
	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);

	return;
}

void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& worldUp) {	
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR t = XMLoadFloat3(&target);
	XMVECTOR u = XMLoadFloat3(&worldUp);

	LookAt(p, t, u);

	return;
}

XMMATRIX Camera::GetViewMatrix() const {
	return XMLoadFloat4x4(&m_view);
}

XMMATRIX Camera::GetProjectionMatrix() const {
	return XMLoadFloat4x4(&m_proj);
}

XMMATRIX Camera::GetViewProjectionMatrix() const {
	XMMATRIX view = XMLoadFloat4x4(&m_view);
	XMMATRIX proj = XMLoadFloat4x4(&m_proj);

	return view * proj;
}

void Camera::Strafe(float d) {
	XMVECTOR distance = XMVectorReplicate(d);
	XMVECTOR right = XMLoadFloat3(&m_right);
	XMVECTOR position = XMLoadFloat3(&m_position);

	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(distance, right, position));

	return;
}

void Camera::Walk(float d) {
	XMVECTOR distance = XMVectorReplicate(d);
	XMVECTOR look = XMLoadFloat3(&m_look);
	XMVECTOR position = XMLoadFloat3(&m_position);

	XMStoreFloat3(&m_position, XMVectorMultiplyAdd(distance, look, position));

	return;
}

void Camera::Pitch(float angle) {
	// Rotation matrix
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&m_right), angle);

	// Recalculate the up and look at vectors
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), r));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), r));

	return;
}

void Camera::RotateY(float angle) {
	// Rotation matrix
	XMMATRIX r = XMMatrixRotationY(angle);

	// Recalculate the up, right and look at vectors
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), r));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), r));
	XMStoreFloat3(&m_look, XMVector3TransformNormal(XMLoadFloat3(&m_look), r));

	return;
}

void Camera::UpdateViewMatrix() {
	XMVECTOR right = XMLoadFloat3(&m_right);
	XMVECTOR up = XMLoadFloat3(&m_up);
	XMVECTOR look = XMLoadFloat3(&m_look);
	XMVECTOR position = XMLoadFloat3(&m_position);

	// Make sure all vectors have 90 degree angles and are of unit length
	look = XMVector3Normalize(look);
	up = XMVector3Normalize(XMVector3Cross(look, right));
	right = XMVector3Cross(up, look);

	// Create the view matrix that has the following form
	//          --             --
	//          | x1  y1  z1  0 |
	// m_view = | x2  y2  z2  0 |
	//          | x3  y3  z3  0 |
	//          | -px -py -pz 1 |
	//          --             --
	// Where x, y, and z represent the respective axis and p is the eye position
	float x = -XMVectorGetX(XMVector3Dot(position, right));
	float y = -XMVectorGetX(XMVector3Dot(position, up));
	float z = -XMVectorGetX(XMVector3Dot(position, look));

	XMStoreFloat3(&m_right, right);
	XMStoreFloat3(&m_up, up);
	XMStoreFloat3(&m_look, look);

	m_view(0, 0) = m_right.x;
	m_view(1, 0) = m_right.y;
	m_view(2, 0) = m_right.z;
	m_view(3, 0) = x;

	m_view(0, 1) = m_up.x;
	m_view(1, 1) = m_up.y;
	m_view(2, 1) = m_up.z;
	m_view(3, 1) = y;

	m_view(0, 2) = m_look.x;
	m_view(1, 2) = m_look.y;
	m_view(2, 2) = m_look.z;
	m_view(3, 2) = z;

	m_view(0, 3) = 0.0f;
	m_view(1, 3) = 0.0f;
	m_view(2, 3) = 0.0f;
	m_view(3, 3) = 1.0f;

	return;
}