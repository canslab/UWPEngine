#pragma once
#include "IDrawable.h"

class CGameObject;
class CCamera;

class CGameWorld : public IDrawable
{
public:
	bool Initialize(CCamera *pMainCamera);
	bool IsInitialized() const;
	bool AddObject(CGameObject* pDrawableObject);
	bool AddCamera(CCamera* pCamera);
	bool ChangeMainCameraTo(unsigned int index);
	
	const std::vector<CGameObject*>* GetGameObjectList() const;
	const CCamera* GetMainCamera() const;
	bool CopyTo(CGameWorld& target) const;

	// ICompositeDrawable Implementation
	virtual int										GetNumberOfDrawableObject() const;
	virtual std::vector<unsigned int>				GetIndexesOfVertexBuffer() const;
	virtual std::vector<unsigned int>				GetIndexesOfIndexBuffer() const;

	// IDrawable Implementation
	virtual unsigned int							GetVertexByteSize() const;
	virtual unsigned int							GetVertexCount() const;
	virtual void*									GetAddressOfVertexArray() const;
	virtual unsigned int							GetIndexCount() const;
	virtual void*									GetAddressOfIndexArray() const;

public:
	CGameWorld();
	virtual ~CGameWorld();

	CGameWorld(const CGameWorld &world) = delete;
	CGameWorld&	operator=(const CGameWorld&) = delete;

protected:
	void _ReleaseCameraListAndResetMainCamera();
	void _ReleaseObjectList();

protected:
	const CCamera*				m_pMainCamera;

	std::vector<CCamera*>*		m_pCameraList;
	std::vector<CGameObject*>*	m_pObjectList;
	bool						m_bInitialized;
};

