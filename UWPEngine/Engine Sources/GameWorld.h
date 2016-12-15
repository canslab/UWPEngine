#pragma once
#include "IDrawable.h"

class CGameObject;
class CCamera;

class CGameWorld : public IDrawable
{
public:
	virtual bool Initialize();
	virtual bool AddObject(CGameObject* pDrawableObject);

	virtual bool UpdateAll();
	virtual bool Update(CGameObject *pUpdatedGameObject);

	const std::vector<CGameObject*>& GetGameObjectList() const;
	CCamera& GetCamera();

	// IDrawable Implementation
	virtual unsigned int								GetNumberOfDrawableObject() const;
	virtual const void*									GetContVertexArray() const;
	virtual const void*									GetContIndexArray() const;
	virtual unsigned int								GetIndexCount() const;
	virtual const std::vector<unsigned int>&			GetIndicesOfVertexBuffer() const;
	virtual const std::vector<unsigned int>&			GetIndicesOfIndexBuffer() const;
	virtual unsigned int 								GetTotalVertexBufferSizeInByte() const;
	virtual unsigned int								GetTotalIndexBufferSizeInByte() const;
	virtual unsigned int								GetVertexStride() const;
	virtual const std::vector<DirectX::XMFLOAT4X4>&		GetWorldMatrices() const;
	virtual DirectX::XMFLOAT4X4							GetCameraMatrix() const;

public:
	CGameWorld();
	virtual ~CGameWorld();

	CGameWorld(const CGameWorld &world) = delete;
	CGameWorld&	operator=(const CGameWorld&) = delete;

protected:
	void _ReleaseObjectList();
	bool _LoadMeshFileAndSaveThemToGlobalBuffer(const char *pMeshFileName);

	std::set<std::string> m_meshFileNameSet;
protected:
	CCamera								m_camera;
	std::vector<CGameObject*>			m_objectList;
	std::vector<DirectX::XMFLOAT4X4>	m_objectWorldMatrices;
	bool								m_bInitialized;

	std::vector<CGameObject::ModelVertex>	m_globalVertexBufferInSystemMemory;
	std::vector<UINT>						m_globalIndexBufferInSystemMemory;
	std::vector<unsigned int>				m_indicesOfGlobalVertexBuffer;
	std::vector<unsigned int>				m_indicesOfGlobalIndexBuffer;

	unsigned int						m_nIndexCount;
};

