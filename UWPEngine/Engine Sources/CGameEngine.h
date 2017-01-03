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

	void Process() const;
	void SetWorld(CGameWorld *pWorld);

	void RenderFont(int x, int y, int width, int height, const wchar_t* pFontType, const float fontSize, const wchar_t *pFont);

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


