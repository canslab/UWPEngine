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
	m_gVBInSystemMemory(0),
	m_gIBInSystemMemory(0),
	m_gVBIndices(0),
	m_gIBIndicies(0),
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
	m_bInitialized = m_camera.Initialize({ 0,0,0 }, { 0,0,0 }, { 0,1,0 });

	m_gIBIndicies.push_back(0);
	m_gVBIndices.push_back(0);
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

	_SaveToGlobalBuffers(pGameObject);

	// finally, game object should be registered to this world.
	m_objectList.push_back(pGameObject);

	// add pGameObject's world matrix into m_objectWorldMatrices
	m_objectWorldMatrices.push_back(pGameObject->GetWorldMatrix());

	// the size of our vectors should be equal to the # of their contents.
	m_objectList.shrink_to_fit();
	m_gVBIndices.shrink_to_fit();
	m_gIBIndicies.shrink_to_fit();
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
	return m_gVBIndices;
}

const std::vector<unsigned int>& CGameWorld::GetIndicesOfIndexBuffer() const
{
	assert(m_bInitialized == true);
	return m_gIBIndicies;
}

const void* CGameWorld::GetContVertexArray() const
{
	assert(m_bInitialized == true);
	assert(m_gVBInSystemMemory.size() >= 1);

	return &(m_gVBInSystemMemory[0]);
}

const void* CGameWorld::GetContIndexArray() const
{
	assert(m_bInitialized == true);
	assert(m_gIBInSystemMemory.size() >= 1);

	return &(m_gIBInSystemMemory[0]);
}

unsigned int CGameWorld::GetIndexCount() const
{
	assert(m_bInitialized == true);
	return m_nIndexCount;
}

unsigned int CGameWorld::GetTotalVertexBufferSizeInByte() const
{
	assert(m_bInitialized == true);
	return m_gVBInSystemMemory.size() * sizeof(CGameObject::ModelVertex);
}

unsigned int CGameWorld::GetTotalIndexBufferSizeInByte() const
{
	assert(m_bInitialized == true);
	return m_gIBInSystemMemory.size() * sizeof(UINT);
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

IDrawable::DrawInfo CGameWorld::operator[](UINT idx) const
{
	assert(idx < m_objectList.size());
	IDrawable::DrawInfo retInfo;

	auto curMeshName = m_objectList[idx]->GetMeshFileName();
	retInfo = m_lookUpTable.at(curMeshName);

	return retInfo;
}

std::vector<D3D11_INPUT_ELEMENT_DESC> CGameWorld::GetInputElementDescArray() const
{
	vector<D3D11_INPUT_ELEMENT_DESC> retVector;
	
	// describe how vertex data looks like
	retVector.push_back(
		
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	
	);

	return retVector;
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

bool CGameWorld::_SaveToGlobalBuffers(CGameObject *pGameObject)
{
	assert(pGameObject != nullptr);
	if (pGameObject == nullptr)
	{
		return false;
	}
	if (pGameObject->GetMeshFileName() == nullptr)
	{
		return false;
	}

	auto meshFileName = pGameObject->GetMeshFileName();

	if (m_lookUpTable.count(meshFileName) == 1)
	{
		return true;
	}

	UINT addedIndexCount = 0;

	// 없다면 아래 코드 실행
	std::ifstream meshFileStream(meshFileName, std::ios::in | std::ios::binary);
	if (meshFileStream.is_open())
	{
		while (!meshFileStream.eof())
		{
			char line[100];
			meshFileStream.getline(line, sizeof(line));

			std::istringstream lineStream(line);
			std::string token;

			while (!lineStream.eof())
			{
				lineStream >> token;
				if (token == "v")	// it means this sentence is about vertex information
				{
					char carriageReturn;
					float x, y, z;
					lineStream >> x >> y >> z;
					lineStream >> carriageReturn;

					m_gVBInSystemMemory.push_back({ x,y,z });
				}
				else if (token == "f")
				{
					UINT vertexIndex1, vertexIndex2, vertexIndex3;
					char carriageReturn;
					std::string ignore;

					lineStream >> vertexIndex1 >> ignore;
					lineStream >> vertexIndex2 >> ignore;
					lineStream >> vertexIndex3 >> ignore;
					lineStream >> carriageReturn;

					m_gIBInSystemMemory.push_back(vertexIndex1 - 1);
					m_gIBInSystemMemory.push_back(vertexIndex2 - 1);
					m_gIBInSystemMemory.push_back(vertexIndex3 - 1);

					addedIndexCount += 3;
				}

				else
				{
					break;
				}
			}
		}

		meshFileStream.close();
		m_lookUpTable[pGameObject->GetMeshFileName()] = { addedIndexCount, m_gIBIndicies.back(), m_gVBIndices.back() };

		m_nIndexCount += addedIndexCount;
		m_gVBIndices.push_back(m_gVBIndices.back() + m_gVBInSystemMemory.size());
		m_gIBIndicies.push_back(m_gIBIndicies.back() + m_gIBInSystemMemory.size());
	}
	else
	{
		m_bInitialized = false;
		return false;
	}

	return true;
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