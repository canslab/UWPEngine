#include "pch.h"
#include "GameObject.h"

using namespace std;
using namespace DirectX;

CGameObject::CGameObject()
{
	//m_pVertexList = new std::vector<ModelVertex>();
	//m_pIndexList = new std::vector<unsigned int>();
	m_bInitialized = false;
	m_nCurrentCount = 0;
	m_transform = { 0,0,0,1,1,1 };
}

CGameObject::~CGameObject()
{
	//if (m_pVertexList)
	//{
	//	delete m_pVertexList;
	//}
	m_bInitialized = false;
}

//	
//unsigned int CGameObject::GetVertexByteSize() const
//{
//	return sizeof(ModelVertex);
//}
//
//unsigned int CGameObject::GetVertexCount() const
//{
//	assert(m_pVertexList != nullptr);
//	return m_pVertexList->size();
//}
//
//void * CGameObject::GetAddressOfVertexArray() const
//{
//	assert(m_pVertexList != nullptr && m_pVertexList->size() >= 1);
//
//	return &(m_pVertexList->at(0));
//}
//
//unsigned int CGameObject::GetIndexCount() const
//{
//	int retValue = m_pIndexList->size();
//
//	return retValue;
//}
//
//unsigned int CGameObject::GetIndexByteSize() const
//{
//	return sizeof(UINT);
//}
//
//void * CGameObject::GetAddressOfIndexArray() const
//{
//	assert(m_pIndexList != nullptr && m_pIndexList->size() >= 1);
//
//	return &(m_pIndexList->at(0));
//}

bool CGameObject::Initialize(const string& meshFileName)
{
	assert(m_bInitialized == false);
	m_pMeshFileName = meshFileName.c_str();

	

	m_bInitialized = true;
	//m_pVertexList->shrink_to_fit();
	//m_pIndexList->shrink_to_fit();
	return m_bInitialized;
}

void CGameObject::SetPositionW(const std::vector<float>& positionW)
{
	assert(m_bInitialized == true && positionW.size() == 4 && positionW[3] == 1);

	m_transform.x = positionW[0];
	m_transform.y = positionW[1];
	m_transform.z = positionW[2];
}

void CGameObject::SetScale(const std::vector<float>& scales)
{
	assert(m_bInitialized == true && scales.size() == 3);
	m_transform.xScale = scales[0];
	m_transform.yScale = scales[1];
	m_transform.zScale = scales[2];
}

void CGameObject::SetRotation(const std::vector<float>& rotationAngles)
{
	assert(m_bInitialized == true && rotationAngles.size() == 3);
	m_transform.xRotAngle = rotationAngles[0];
	m_transform.yRotAngle = rotationAngles[1];
	m_transform.zRotAngle = rotationAngles[2];
}

bool CGameObject::IsInitalized() const
{
	return m_bInitialized;
}

XMFLOAT4 CGameObject::GetPosition() const
{
	assert(m_bInitialized == true);
	return XMFLOAT4(m_transform.x, m_transform.y, m_transform.z, 1);
}

XMFLOAT3 CGameObject::GetScale() const
{
	return XMFLOAT3(m_transform.xScale, m_transform.yScale, m_transform.zScale);
}

const char * CGameObject::GetMeshFileName() const
{
	return m_pMeshFileName;
}

XMFLOAT4X4 CGameObject::GetWorldMatrix() const
{
	assert(m_bInitialized == true);

	XMFLOAT4X4 retMatrix;

	auto scalingMatrix = XMMatrixScaling(m_transform.xScale, m_transform.yScale, m_transform.zScale);
	auto rotationMatrix = XMMatrixRotationX(m_transform.xRotAngle) * XMMatrixRotationY(m_transform.yRotAngle) *XMMatrixRotationZ(m_transform.zRotAngle);
	auto translationMatrix = XMMatrixTranslation(m_transform.x, m_transform.y, m_transform.z);

	// S * R * T
	auto resultWorldMatrix = scalingMatrix * rotationMatrix * translationMatrix;
	XMStoreFloat4x4(&retMatrix, resultWorldMatrix);
	
	return retMatrix;
}
