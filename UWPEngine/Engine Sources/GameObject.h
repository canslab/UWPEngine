#pragma once
#include "IDrawable.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	DirectX::XMFLOAT4X4			GetWorldMatrix() const;

	bool						Initialize(std::string meshFileName);
	void						SetPositionW(float xPos, float yPos, float zPos);
	void						SetScale(float xScale, float yScale, float zScale);
	void						SetRotation(float xRotAngle, float yRotAngle,float zRotAngle);

	bool						IsInitalized() const;
	DirectX::XMFLOAT4			GetPosition() const;
	DirectX::XMFLOAT3			GetScale() const;
	
	const char*					GetMeshFileName() const;

	struct ModelVertex
	{
		float x, y, z;
		ModelVertex() :x(0), y(0), z(0) {}
		ModelVertex(float a, float b, float c) :x(a), y(b), z(c) {}
	};

protected:
	struct Transform 
	{
		float x, y, z;
		float xScale, yScale, zScale;
		float xRotAngle, yRotAngle, zRotAngle;
	};

	int							m_nCurrentCount;
	
	Transform					m_transform;		// transform information
	char						m_pMeshFileName[50];		// mesh File name for lazy evaluation
	bool						m_bInitialized;		// whether Game Object is initialized or not

	//std::vector<ModelVertex>*	m_pVertexList;		// vertexs that composes a Game Object
	//std::vector<unsigned int>*	m_pIndexList;		// index list that composes a Game Object
};