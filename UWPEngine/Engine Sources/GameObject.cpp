#include "pch.h"
#include "GameObject.h"

using namespace std;
using namespace DirectX;

CGameObject::CGameObject()
{
	m_pVertexList = new std::vector<ModelVertex>();
	m_pIndexList = new std::vector<ModelIndex>();
	m_bInitialized = false;
	m_nCurrentCount = 0;
	m_transform = { 0,0,0,1,1,1 };
}

CGameObject::~CGameObject()
{
	if (m_pVertexList)
	{
		delete m_pVertexList;
	}
	m_bInitialized = false;
}

bool CGameObject::GetNextVertex(float * resultX, float * resultY, float * resultZ)
{
	assert(resultX != nullptr && resultY != nullptr && resultZ != nullptr);
	assert(m_pVertexList != nullptr);

	if (m_nCurrentCount < (m_pVertexList->size()))
	{
		*resultX = m_pVertexList->at(m_nCurrentCount).x;
		*resultY = m_pVertexList->at(m_nCurrentCount).y;
		*resultZ = m_pVertexList->at(m_nCurrentCount).z;

		m_nCurrentCount++;
		return true;
	}
	else
	{
		m_nCurrentCount = 0;
		return false;
	}
}

unsigned int CGameObject::GetVertexByteSize() const
{
	return sizeof(ModelVertex);
}

unsigned int CGameObject::GetVertexCount() const
{
	assert(m_pVertexList != nullptr);
	return m_pVertexList->size();
}

void * CGameObject::GetAddressOfVertexArray() const
{
	assert(m_pVertexList != nullptr && m_pVertexList->size() >= 1);

	return &(m_pVertexList->at(0));
}

unsigned int CGameObject::GetIndexCount() const
{
	int retValue = m_pIndexList->size() * (sizeof(ModelIndex) / sizeof(UINT));

	return retValue;
}

void * CGameObject::GetAddressOfIndexArray() const
{
	assert(m_pIndexList != nullptr && m_pIndexList->size() >= 1);

	return &(m_pIndexList->at(0));
}

bool CGameObject::Initialize(const string& meshFileName)
{
	assert(m_bInitialized == false);

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
					float v1, v2, v3;
					lineStream >> v1 >> v2 >> v3;
					lineStream >> carriageReturn;

					m_pVertexList->push_back({ v1, v2, v3 });
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

					m_pIndexList->push_back({ vertexIndex1 - 1, vertexIndex2 - 1, vertexIndex3 - 1 });
				}

				else
				{
					break;
				}
			}
		}
	}
	else
	{
		m_bInitialized = false;
		return false;
	}

	m_bInitialized = true;
	m_pVertexList->shrink_to_fit();
	m_pIndexList->shrink_to_fit();
	return true;
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

XMMATRIX CGameObject::GetWorldMatrix() const
{
	assert(m_bInitialized == true);
	auto scalingMatrix = XMMatrixScaling(m_transform.xScale, m_transform.yScale, m_transform.zScale);
	// auto rotationMatrix;
	auto translationMatrix = XMMatrixTranslation(m_transform.x, m_transform.y, m_transform.z);

	return scalingMatrix * translationMatrix;
}
