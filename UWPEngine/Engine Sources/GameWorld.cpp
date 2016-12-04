#include "pch.h"
#include "GameWorld.h"
#include "GameObject.h"
#include "CCamera.h"

using namespace std;

CGameWorld::CGameWorld() :
	m_pMainCamera(nullptr),
	m_pCameraList(nullptr),
	m_pObjectList(nullptr),
	m_bInitialized(false)
{

}

CGameWorld::~CGameWorld()
{
	_ReleaseCameraListAndResetMainCamera();
	_ReleaseObjectList();
	m_pMainCamera = nullptr;
	m_bInitialized = false;
}

bool CGameWorld::Initialize(CCamera *pMainCamera)
{
	assert(m_bInitialized == false && pMainCamera != nullptr && pMainCamera->IsInitialized());

	if (m_bInitialized == true)
	{
		return false;
	}

	m_pCameraList = new vector<CCamera*>();
	if (m_pCameraList == nullptr)
	{
		__debugbreak();
		m_bInitialized = false;
		return false;
	}

	m_pObjectList = new vector<CGameObject*>();
	if (m_pObjectList == nullptr)
	{
		__debugbreak();
		m_bInitialized = false;
		_ReleaseCameraListAndResetMainCamera();
		return false;
	}

	// add at least one camera, it is set to the main camera by default.
	m_pCameraList->push_back(pMainCamera);
	m_pCameraList->shrink_to_fit();
	m_pMainCamera = pMainCamera;

	// initialized well means allocation of the camera list and the object list is done successfully
	// also main camera is set.
	m_bInitialized = true;

	return true;
}

bool CGameWorld::IsInitialized() const
{
	return m_bInitialized;
}

bool CGameWorld::AddObject(CGameObject * pGameObject)
{
	assert(m_bInitialized == true && pGameObject != nullptr && pGameObject->IsInitalized() == true);

	if (m_bInitialized == false || pGameObject == nullptr || pGameObject->IsInitalized() == false)
	{
		return false;
	}

	m_pObjectList->push_back(pGameObject);
	return true;
}

bool CGameWorld::AddCamera(CCamera *pCamera)
{
	assert(m_bInitialized == true && pCamera != nullptr && pCamera->IsInitialized() == true);
	if (m_bInitialized == false || pCamera == nullptr || pCamera->IsInitialized() == false)
	{
		return false;
	}

	m_pCameraList->push_back(pCamera);
	return true;
}

bool CGameWorld::ChangeMainCameraTo(unsigned int index)
{
	assert(m_bInitialized == true && 0 <= index && index < m_pCameraList->size());

	if (m_bInitialized == false || index >= m_pCameraList->size() || index < 0)
	{
		return false;
	}

	m_pMainCamera = m_pCameraList->at(index);

	return true;
}

const std::vector<CGameObject*>* CGameWorld::GetGameObjectList() const
{
	assert(m_bInitialized == true);
	return m_pObjectList;
}

const CCamera* CGameWorld::GetMainCamera() const
{
	assert(m_bInitialized == true);
	return m_pMainCamera;
}

bool CGameWorld::CopyTo(CGameWorld & target) const
{
	//not yet implemented
	return false;
}

int CGameWorld::GetNumberOfDrawableObject() const
{
	assert(m_pObjectList != nullptr);
	return this->m_pObjectList->size();
}

std::vector<unsigned int> CGameWorld::GetIndexesOfVertexBuffer() const
{
	std::vector<unsigned int> retVector;
	unsigned int acc = 0;

	retVector.push_back(0);
	for (auto eachObject : *m_pObjectList)
	{
		auto eachObjectVertexCount = eachObject->GetVertexCount();
		retVector.push_back(acc + eachObjectVertexCount);
		acc += eachObjectVertexCount;
	}

	// for example, in world, there is a only single object which consist of 12 vertices.
	// then retVector = { 12 }

	return retVector;
}

std::vector<unsigned int> CGameWorld::GetIndexesOfIndexBuffer() const
{
	std::vector<unsigned int> retVector;
	unsigned int acc = 0;

	retVector.push_back(0);
	for (auto eachObject : *m_pObjectList)
	{
		auto eachObjectIndexCount = eachObject->GetIndexCount();
		retVector.push_back(acc + eachObjectIndexCount);
		acc += eachObjectIndexCount;
	}

	// for example, in world, there is a only single object which consist of 12 indices.
	// then retVector = { 12 }

	return retVector;
}

unsigned int CGameWorld::GetVertexByteSize() const
{
	return m_pObjectList->at(0)->GetVertexByteSize();
}

unsigned int CGameWorld::GetVertexCount() const
{
	unsigned int nVertexCount = 0;

	for (auto eachObject : *m_pObjectList)
	{
		auto eachObjectVertexCount = eachObject->GetVertexCount();
		nVertexCount += eachObjectVertexCount;
	}
	return nVertexCount;
}

void* CGameWorld::GetAddressOfVertexArray() const
{
	// for example 
	// object1 
	// # of vertex = 6
	// each vertex byte size = 10
	// total 60 bytes
	/*unique_ptr<vector<char>> pRet(new vector<char>());
	unsigned int acc = 0;

	for (auto pEachObject : *m_pObjectList)
	{
		unsigned int nEachVertexCount = 0;
		unsigned int nEachVertexByteSize = 0;
		unsigned int addedSize = 0;
		unique_ptr<vector<char>> pTempVertexArray;

		pTempVertexArray = pEachObject->GetAddressOfVertexArray();
		nEachVertexCount = pEachObject->GetVertexCount();
		nEachVertexByteSize = pEachObject->GetVertexByteSize();
		addedSize = sizeof(char) * nEachVertexByteSize * nEachVertexCount;

		pRet->resize(acc + addedSize);
		memcpy(&(pRet->at(acc)), &(pTempVertexArray->at(0)), addedSize);
		acc += addedSize;
	}

	return pRet;*/
	return nullptr;
}

unsigned int CGameWorld::GetIndexCount() const
{
	unsigned int nIndexCount = 0;

	for (auto eachObject : *m_pObjectList)
	{
		auto eachObjectIndexCount = eachObject->GetIndexCount();
		nIndexCount += eachObjectIndexCount;
	}
	return nIndexCount;
}

void* CGameWorld::GetAddressOfIndexArray() const
{
	// for example 
	// object1 
	// # of vertex = 6
	// each vertex byte size = 10
	// total 60 bytes
	/*unique_ptr<vector<char>> pRet(new vector<char>());
	unsigned int acc = 0;

	for (auto pEachObject : *m_pObjectList)
	{
		unsigned int nEachVertexCount = 0;
		unsigned int nEachVertexByteSize = 0;
		unsigned int addedSize = 0;
		unique_ptr<vector<char>> pTempIndexArray;

		pTempIndexArray = pEachObject->GetAddressOfIndexArray();
		addedSize = sizeof(UINT) * pEachObject->GetIndexCount();

		pRet->resize(acc + addedSize);
		memcpy(&(pRet->at(acc)), &(pTempIndexArray->at(0)), addedSize);
		acc += addedSize;
	}
*/
	return nullptr;
}

void CGameWorld::_ReleaseCameraListAndResetMainCamera()
{
	if (m_pCameraList)
	{
		for (auto pEachCameraObject : *m_pCameraList)
		{
			if (pEachCameraObject)
			{
				delete pEachCameraObject;
			}
		}
		delete m_pCameraList;
	}

	m_pCameraList = nullptr;
	m_pMainCamera = nullptr;
}

void CGameWorld::_ReleaseObjectList()
{
	if (m_pObjectList)
	{
		for (auto pEachObject : *m_pObjectList)
		{
			if (pEachObject)
			{
				delete pEachObject;
			}
		}
		delete m_pObjectList;
	}

	m_pObjectList = nullptr;
}
