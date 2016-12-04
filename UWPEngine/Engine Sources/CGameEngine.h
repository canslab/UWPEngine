#pragma once
#include "D3DRenderer.h"
#include "CCamera.h"
#include "GameObject.h"

class CGameEngine
{
public:
	static CGameEngine* GetSharedEngine();

	bool Initialize(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel);
	void UpdateForWindowSizeOrScaleChanged(const Windows::Foundation::Size *pNewLogicalSize, float fCompositionScaleX, float fCompositionScaleY);

	void Process() const;
	void AddObject(CGameObject* object);
	void AddCamera(CCamera *camera);
	
	virtual ~CGameEngine();

	// Prohibite Copy, Assignment
	CGameEngine& operator=(CGameEngine& srcEngine) = delete;
	CGameEngine(CGameEngine &src) = delete;

private:
	CGameEngine();

private:
	CD3DRenderer*					m_pRenderer;
	std::vector<CCamera*>*			m_pCameraList;
	std::vector<CGameObject*>*		m_pObjectList;
};


