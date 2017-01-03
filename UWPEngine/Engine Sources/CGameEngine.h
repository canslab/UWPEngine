#pragma once
#include "D3DRenderer.h"
#include "CCamera.h"
#include "GameObject.h"
#include "GameWorld.h"
#include "GameTimer.h"

class CGameEngine
{
public:
	static CGameEngine* GetSharedEngine();

	bool Initialize(Windows::UI::Xaml::Controls::SwapChainPanel^ swapChainPanel);
	void UpdateForWindowSizeOrScaleChanged(const Windows::Foundation::Size *pNewLogicalSize, float fCompositionScaleX, float fCompositionScaleY);

	void UpdateContents() const;
	void Present() const;
	void SetWorld(CGameWorld *pWorld);

	void DrawText(float x, float y, float width, float height, const wchar_t* pFontType, const wchar_t *pFont);

	virtual ~CGameEngine();

	// Prohibite Copy, Assignment
	CGameEngine& operator=(CGameEngine& srcEngine) = delete;
	CGameEngine(CGameEngine &src) = delete;

private:
	CGameEngine();

private:
	CD3DRenderer*					m_pRenderer;
	CGameWorld*						m_pGameWorld;
};


