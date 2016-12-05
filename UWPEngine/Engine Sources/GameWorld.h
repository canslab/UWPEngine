#pragma once
#include "IDrawable.h"

class CGameObject;
class CCamera;

class CGameWorld : public IDrawable
{
public:
	bool Initialize();
	bool AddObject(CGameObject* pDrawableObject);
	void SetCameraPositionTo(const float positionW[3], const float targetPositionW[3], const float upVectorW[3]);

	const std::vector<CGameObject*>* GetGameObjectList() const;
	const CCamera* GetCamera() const;

	// IDrawable Implementation
	virtual unsigned int							GetNumberOfDrawableObject() const;
	virtual const void*								GetContVertexArray() const;
	virtual const void*								GetContIndexArray() const;
	virtual unsigned int							GetIndexCount() const;
	virtual const std::vector<unsigned int>*		GetIndicesOfVertexBuffer() const;
	virtual const std::vector<unsigned int>*		GetIndicesOfIndexBuffer() const;
	virtual unsigned int 							GetTotalVertexBufferSizeInByte() const;
	virtual unsigned int							GetTotalIndexBufferSizeInByte() const;
	virtual unsigned int							GetVertexStride() const;
	virtual std::vector<DirectX::XMFLOAT4X4>		GetWorldMatrices() const;
	virtual DirectX::XMFLOAT4X4						GetCameraMatrix() const;

public:
	CGameWorld();
	virtual ~CGameWorld();

	CGameWorld(const CGameWorld &world) = delete;
	CGameWorld&	operator=(const CGameWorld&) = delete;

protected:
	void _ReleaseObjectList();

protected:
	CCamera						m_camera;
	std::vector<CGameObject*>	m_objectList;
	bool						m_bInitialized;

	std::vector<unsigned char>	m_globalVertexBufferInSystemMemory;
	std::vector<unsigned char>	m_globalIndexBufferInSystemMemory;
	std::vector<unsigned int>	m_indicesOfGlobalVertexBuffer;
	std::vector<unsigned int>	m_indicesOfGlobalIndexBuffer;

	unsigned int				m_totalIndexCount;
};

