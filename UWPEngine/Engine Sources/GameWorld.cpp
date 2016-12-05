#include "pch.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "CCamera.h"

using namespace std;
using namespace DirectX;

CGameWorld::CGameWorld() :
	m_camera(),
	m_objectList(0),
	m_globalVertexBufferInSystemMemory(0),
	m_globalIndexBufferInSystemMemory(0),
	m_indicesOfGlobalVertexBuffer(0),
	m_indicesOfGlobalIndexBuffer(0),
	m_bInitialized(false),
	m_totalIndexCount(0)
{

}	

CGameWorld::~CGameWorld()
{
	_ReleaseObjectList();
}

bool CGameWorld::Initialize()
{
	assert(m_bInitialized == false);
	m_bInitialized = m_camera.Initialize({ 0,0,0 ,1 }, { 0,0,0,1 }, { 0,1,0,0 });

	m_indicesOfGlobalIndexBuffer.push_back(0);
	m_indicesOfGlobalVertexBuffer.push_back(0);

	return m_bInitialized;
}

bool CGameWorld::AddObject(CGameObject * pGameObject)
{
	assert(m_bInitialized == true && pGameObject != nullptr && pGameObject->IsInitalized() == true);

	static unsigned int currentIndiciesOfVertexBuffer = 0;
	static unsigned int currentIndiciesOfIndexBuffer = 0;

	if (m_bInitialized == false || pGameObject == nullptr || pGameObject->IsInitalized() == false)
	{
		return false;
	}

	m_objectList.push_back(pGameObject);
	// make ensure that the size of m_objectList is equal to the size of game object.
	m_objectList.shrink_to_fit();

	auto addedObjectVertexBufferSizeInByte = pGameObject->GetVertexCount() * pGameObject->GetVertexByteSize();
	auto addedObjectIndexBufferSizeInByte = pGameObject->GetIndexCount() * pGameObject->GetIndexByteSize();
		
	m_totalIndexCount += pGameObject->GetIndexCount();

	currentIndiciesOfVertexBuffer += pGameObject->GetVertexCount();
	currentIndiciesOfIndexBuffer += pGameObject->GetIndexCount();

	m_indicesOfGlobalVertexBuffer.push_back(currentIndiciesOfVertexBuffer);
	m_indicesOfGlobalIndexBuffer.push_back(currentIndiciesOfIndexBuffer);
	m_indicesOfGlobalVertexBuffer.shrink_to_fit();
	m_indicesOfGlobalIndexBuffer.shrink_to_fit();

	m_globalVertexBufferInSystemMemory.resize(m_globalVertexBufferInSystemMemory.size() + addedObjectVertexBufferSizeInByte);
	m_globalIndexBufferInSystemMemory.resize(m_globalIndexBufferInSystemMemory.size() + addedObjectIndexBufferSizeInByte);
	
	// copy to CGameWorld's internal memory
	memcpy(&(m_globalVertexBufferInSystemMemory[m_globalVertexBufferInSystemMemory.size() - addedObjectVertexBufferSizeInByte]), pGameObject->GetAddressOfVertexArray(), addedObjectVertexBufferSizeInByte);
	memcpy(&(m_globalIndexBufferInSystemMemory[m_globalIndexBufferInSystemMemory.size() - addedObjectIndexBufferSizeInByte]), pGameObject->GetAddressOfIndexArray(), addedObjectIndexBufferSizeInByte);

	return true;
}

void CGameWorld::SetCameraPositionTo(const float positionW[3], const float targetPositionW[3], const float upVectorW[3])
{
	assert(m_bInitialized == true && positionW != nullptr && targetPositionW != nullptr && upVectorW != nullptr);

	m_camera.SetPositionW(positionW[0], positionW[1], positionW[2]);
	m_camera.SetTargetPositionW(targetPositionW[0], targetPositionW[1], targetPositionW[2]);
	m_camera.SetUpVectorW(upVectorW[0], upVectorW[1], upVectorW[2]);
}

const std::vector<CGameObject*>* CGameWorld::GetGameObjectList() const
{
	assert(m_bInitialized == true);
	return &m_objectList;
}

const CCamera* CGameWorld::GetCamera() const
{
	assert(m_bInitialized == true);
	return &m_camera;
}

unsigned int CGameWorld::GetNumberOfDrawableObject() const
{
	assert(m_bInitialized == true);
	return m_objectList.size();
}

const std::vector<unsigned int>* CGameWorld::GetIndicesOfVertexBuffer() const
{
	assert(m_bInitialized == true);
	return &m_indicesOfGlobalVertexBuffer;
}

const std::vector<unsigned int>* CGameWorld::GetIndicesOfIndexBuffer() const
{
	assert(m_bInitialized == true);
	return &m_indicesOfGlobalIndexBuffer;
}

const void* CGameWorld::GetContVertexArray() const
{
	assert(m_bInitialized == true);
	assert(m_globalVertexBufferInSystemMemory.size() >= 1);
	
	return &(m_globalVertexBufferInSystemMemory[0]);
}

const void* CGameWorld::GetContIndexArray() const
{
	assert(m_bInitialized == true);
	assert(m_globalIndexBufferInSystemMemory.size() >= 1);

	return &(m_globalIndexBufferInSystemMemory[0]);
}

unsigned int CGameWorld::GetIndexCount() const
{
	assert(m_bInitialized == true);
	return m_totalIndexCount;
}

unsigned int CGameWorld::GetTotalVertexBufferSizeInByte() const
{
	assert(m_bInitialized == true);
	return m_globalVertexBufferInSystemMemory.size();
}

unsigned int CGameWorld::GetTotalIndexBufferSizeInByte() const
{
	assert(m_bInitialized == true);
	return m_globalIndexBufferInSystemMemory.size();
}

unsigned int CGameWorld::GetVertexStride() const
{
	return sizeof(CGameObject::ModelVertex);
}

vector<XMFLOAT4X4> CGameWorld::GetWorldMatrices() const
{
	vector<XMFLOAT4X4> retMatrices;
	for (auto pEachObject : m_objectList)
	{
		auto eachWorldMatrix = pEachObject->GetWorldMatrix();

		retMatrices.push_back(eachWorldMatrix);
	}

	return retMatrices;
}

DirectX::XMFLOAT4X4 CGameWorld::GetCameraMatrix() const
{
	assert(m_bInitialized == true);
	return m_camera.GetViewMatrix();
}


void CGameWorld::_ReleaseObjectList()
{
	assert(m_bInitialized == true);

	for (auto pEachObject : m_objectList)
	{
		if (pEachObject)
		{
			delete pEachObject;
		}
	}
	m_objectList.clear();
}
