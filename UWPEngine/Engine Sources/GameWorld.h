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
	unsigned int										GetNumberOfDrawableObject()			const override;
	const void*											GetContVertexArray()				const override;
	const void*											GetContIndexArray()					const override;
	unsigned int										GetIndexCount()						const override;
	const std::vector<unsigned int>&					GetIndicesOfVertexBuffer()			const override;
	const std::vector<unsigned int>&					GetIndicesOfIndexBuffer()			const override;
	unsigned int 										GetTotalVertexBufferSizeInByte()	const override;
	unsigned int										GetTotalIndexBufferSizeInByte()		const override;
	unsigned int										GetVertexStride()					const override;
	const std::vector<DirectX::XMFLOAT4X4>&				GetWorldMatrices()					const override;
	DirectX::XMFLOAT4X4									GetCameraMatrix()					const override;

	virtual DrawInfo									operator[](UINT idx)				const override;
	virtual std::vector<D3D11_INPUT_ELEMENT_DESC>		GetInputElementDescArray()			const override;

public:
	CGameWorld();
	virtual ~CGameWorld();

	CGameWorld(const CGameWorld &world) = delete;
	CGameWorld&	operator=(const CGameWorld&) = delete;

protected:
	void _ReleaseObjectList();
	bool _SaveToGlobalBuffers(CGameObject *pGameObject);

protected:

	CCamera													m_camera;
	std::vector<CGameObject*>								m_objectList;
	std::vector<DirectX::XMFLOAT4X4>						m_objectWorldMatrices;
	bool													m_bInitialized;

	std::vector<CGameObject::ModelVertex>					m_gVBInSystemMemory;
	std::vector<UINT>										m_gIBInSystemMemory;
	std::vector<UINT>										m_gVBIndices;
	std::vector<UINT>										m_gIBIndicies;

	std::map<std::string, DrawInfo>							m_lookUpTable;
	unsigned int											m_nIndexCount;
};

