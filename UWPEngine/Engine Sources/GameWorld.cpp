#include "pch.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "CCamera.h"

using namespace std;
using namespace DirectX;

CGameWorld::CGameWorld() :
	m_camera(),
	m_objectList(0),
	m_objectWorldMatrices(0),
	m_bInitialized(false),
	m_globalVertexBufferInSystemMemory(0),
	m_globalIndexBufferInSystemMemory(0),
	m_indicesOfGlobalVertexBuffer(0),
	m_indicesOfGlobalIndexBuffer(0),
	m_nIndexCount(0)
{

}	

CGameWorld::~CGameWorld()
{
	// release all objects in m_objectList
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

	if (m_bInitialized == false || pGameObject == nullptr || pGameObject->IsInitalized() == false)
	{
		// exceptional case 
		return false;
	}

	auto gameObjectVertexCount = pGameObject->GetVertexCount();
	auto gameObjectIndexCount = pGameObject->GetIndexCount();
	auto addedObjectVertexBufferSizeInByte = gameObjectVertexCount * pGameObject->GetVertexByteSize();
	auto addedObjectIndexBufferSizeInByte = gameObjectIndexCount * pGameObject->GetIndexByteSize();
	
	// refresh total Index Count
	m_nIndexCount += gameObjectIndexCount;

	m_indicesOfGlobalVertexBuffer.push_back(m_indicesOfGlobalVertexBuffer.back() + gameObjectVertexCount);
	m_indicesOfGlobalIndexBuffer.push_back(m_indicesOfGlobalIndexBuffer.back() + gameObjectIndexCount);

	// Expand global vertex, index buffer memory  
	auto previousSize = m_globalVertexBufferInSystemMemory.size();
	m_globalVertexBufferInSystemMemory.resize(previousSize + addedObjectVertexBufferSizeInByte);
	memcpy(&(m_globalVertexBufferInSystemMemory[previousSize]), pGameObject->GetAddressOfVertexArray(), addedObjectVertexBufferSizeInByte);

	previousSize = m_globalIndexBufferInSystemMemory.size();
	m_globalIndexBufferInSystemMemory.resize(previousSize + addedObjectIndexBufferSizeInByte);
	memcpy(&(m_globalIndexBufferInSystemMemory[previousSize]), pGameObject->GetAddressOfIndexArray(), addedObjectIndexBufferSizeInByte);

	// finally, game object should be registered to this world.
	m_objectList.push_back(pGameObject);
	
	// add pGameObject's world matrix into m_objectWorldMatrices
	m_objectWorldMatrices.push_back(pGameObject->GetWorldMatrix());

	// the size of our vectors should be equal to the # of their contents.
	m_objectList.shrink_to_fit();
	m_indicesOfGlobalVertexBuffer.shrink_to_fit();
	m_indicesOfGlobalIndexBuffer.shrink_to_fit();
	m_objectWorldMatrices.shrink_to_fit();

	return true;
}



const vector<CGameObject*>& CGameWorld::GetGameObjectList() const
{
	assert(m_bInitialized == true);
	return m_objectList;
}

CCamera& CGameWorld::GetCamera()
{
	assert(m_bInitialized == true);
	return m_camera;
}

unsigned int CGameWorld::GetNumberOfDrawableObject() const
{
	assert(m_bInitialized == true);
	return m_objectList.size();
}

const std::vector<unsigned int>& CGameWorld::GetIndicesOfVertexBuffer() const
{
	assert(m_bInitialized == true);
	return m_indicesOfGlobalVertexBuffer;
}

const std::vector<unsigned int>& CGameWorld::GetIndicesOfIndexBuffer() const
{
	assert(m_bInitialized == true);
	return m_indicesOfGlobalIndexBuffer;
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
	return m_nIndexCount;
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

const vector<XMFLOAT4X4>& CGameWorld::GetWorldMatrices() const
{
	assert(m_bInitialized == true);
	return m_objectWorldMatrices;
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

bool CGameWorld::UpdateAll()
{
	assert(m_bInitialized == true);
	int i = 0;

	if (m_bInitialized == false)
	{
		goto FAILURE;
	}

	for (auto pEachObject : m_objectList)
	{
		m_objectWorldMatrices[i] = pEachObject->GetWorldMatrix();
		i++;
	}

	return true;
FAILURE:
	return false;
}

bool CGameWorld::Update(CGameObject * pUpdatedGameObject)
{
	assert(pUpdatedGameObject != nullptr);
	assert(m_bInitialized == true);

	if (pUpdatedGameObject == nullptr || m_bInitialized == false)
	{
		goto FAILURE;
	}

	bool bExist = false;
	int objectIndex = 0;
	// check whether pUpdatedGameObject is the member of m_objectList
	for (auto pEachObject : m_objectList)
	{
		if (pEachObject == pUpdatedGameObject)
		{
			bExist = true;
			break;
		}
		objectIndex++;
	}

	if (bExist == false)
	{
		goto FAILURE;
	}
	
	// update (objectIndex)th world Matrix 
	m_objectWorldMatrices[objectIndex] = pUpdatedGameObject->GetWorldMatrix();

	return true;
FAILURE:
	return false;
}