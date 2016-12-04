#pragma once
#include "IDrawable.h"

class CGameObject : public IDrawable
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	// IDrawable Interface Implementation 
	bool						GetNextVertex(float *resultX, float *resultY, float *resultZ);
	unsigned int				GetVertexByteSize() const;
	unsigned int				GetVertexCount() const;
	void*						GetAddressOfVertexArray() const;
	unsigned int				GetIndexCount() const;
	void*						GetAddressOfIndexArray() const;
	DirectX::XMMATRIX			GetWorldMatrix() const;

	bool						Initialize(const std::string& meshFileName);
	void						SetPositionW(const std::vector<float> &positionW);
	void						SetScale(const std::vector<float> &scales);

	bool						IsInitalized() const;
	DirectX::XMFLOAT4			GetPosition() const;
	DirectX::XMFLOAT3			GetScale() const;

private:
	struct ModelVertex
	{
		float x, y, z;
		ModelVertex() :x(0), y(0), z(0) {}
		ModelVertex(float a, float b, float c) :x(a), y(b), z(c) {}
	};
	struct ModelIndex
	{
		UINT v1, v2, v3;
		ModelIndex(UINT a, UINT b, UINT c) :v1(a), v2(b), v3(c) {}
	};
	struct Transform 
	{
		float x, y, z;
		float xScale, yScale, zScale;
	};

	int							m_nCurrentCount;
	
	Transform					m_transform;		// transform information
	std::vector<ModelVertex>*	m_pVertexList;		// vertexs that composes a Game Object
	std::vector<ModelIndex>*	m_pIndexList;		// index list that composes a Game Object
	bool						m_bInitialized;		// whether Game Object is initialized or not
};