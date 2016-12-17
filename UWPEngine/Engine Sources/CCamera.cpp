#include "pch.h"
#include "CCamera.h"
using namespace DirectX;

CCamera::CCamera():
	m_bInitialized(false),
	m_positionW(0,0,0,0),
	m_targetW(0,0,0,0),
	m_upW(0,0,0,0)
{
}

CCamera::~CCamera()
{

}

XMFLOAT4 CCamera::GetPositionW() const
{
	return m_positionW;
}

bool CCamera::Initialize(const std::vector<float>& positionW, const std::vector<float>& targetPositionW, const std::vector<float>& upVectorW)
{
	assert(positionW.size() == 4 && targetPositionW.size() == 4 && upVectorW.size() == 4);
	assert(positionW[3] == 1 && targetPositionW[3] == 1 && upVectorW[3] == 0);

	if (positionW.size() == 4 && targetPositionW.size() == 4 && upVectorW.size() == 4)
	{
		SetPositionW(positionW[0], positionW[1], positionW[2]);
		SetTargetPositionW(targetPositionW[0], targetPositionW[1], targetPositionW[2]);
		SetUpVectorW(upVectorW[0], upVectorW[1], upVectorW[2]);
		return m_bInitialized = true;
	}
	else
	{
		return m_bInitialized = false;
	}
}

bool CCamera::IsInitialized() const
{
	return m_bInitialized;
}

void CCamera::SetPositionW(float x, float y, float z)
{
	m_positionW.x = x;
	m_positionW.y = y;
	m_positionW.z = z;
	m_positionW.w = 1;
}

void CCamera::SetTargetPositionW(float x, float y, float z)
{
	m_targetW.x = x;
	m_targetW.y = y;
	m_targetW.z = z;
	m_targetW.w = 1;
}

void CCamera::SetUpVectorW(float x, float y, float z)
{
	m_upW.x = x;
	m_upW.y = y;
	m_upW.z = z;
	m_upW.w = 0;
}

void CCamera::MovePositionBy(float dx, float dy, float dz)
{
	MovePositionByOffsetX(dx);
	MovePositionByOffsetY(dy);
	MovePositionByOffsetZ(dz);
}

void CCamera::MovePositionByOffsetX(float dx)
{
	m_positionW.x += dx;
}

void CCamera::MovePositionByOffsetY(float dy)
{
	m_positionW.y += dy;
}

void CCamera::MovePositionByOffsetZ(float dz)
{
	m_positionW.z += dz;
}

void CCamera::MoveTargetPositionByOffset(float dx, float dy, float dz)
{
	m_targetW.x += dx;
	m_targetW.y += dy;
	m_targetW.z += dz;
}

XMFLOAT4X4 CCamera::GetViewMatrix() const
{
	XMFLOAT4X4 retMatrix;
	auto pos = XMLoadFloat4(&m_positionW);
	auto target = XMLoadFloat4(&m_targetW);
	auto up = XMLoadFloat4(&m_upW);

	XMStoreFloat4x4(&retMatrix, XMMatrixLookAtLH(pos, target, up));
	
	return retMatrix;
}