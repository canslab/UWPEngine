#pragma once
#include "IDrawable.h"

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	unsigned int				GetVertexByteSize() const;
	unsigned int				GetVertexCount() const;
	void*						GetAddressOfVertexArray() const;
	unsigned int				GetIndexCount() const;
	unsigned int				GetIndexByteSize() const;
	void*						GetAddressOfIndexArray() const;
	
	DirectX::XMFLOAT4X4			GetWorldMatrix() const;

	bool						Initialize(const std::string& meshFileName);
	void						SetPositionW(const std::vector<float> &positionW);
	void						SetScale(const std::vector<float> &scales);

	bool						IsInitalized() const;
	DirectX::XMFLOAT4			GetPosition() const;
	DirectX::XMFLOAT3			GetScale() const;

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
	};

	int							m_nCurrentCount;
	
	Transform					m_transform;		// transform information
	std::vector<ModelVertex>*	m_pVertexList;		// vertexs that composes a Game Object
	std::vector<unsigned int>*	m_pIndexList;		// index list that composes a Game Object
	bool						m_bInitialized;		// whether Game Object is initialized or not
};