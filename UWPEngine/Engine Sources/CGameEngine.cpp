#include "pch.h"
#include "CGameEngine.h"
#include <fstream>

using namespace Windows::UI::Xaml::Controls;
using namespace std;

// think of m_pCameraList as vector<Camera*>

CGameEngine::CGameEngine() :
	m_pRenderer(new CD3DRenderer())
{
	// now global pointer points to this object
}

CGameEngine::~CGameEngine()
{
	if (m_pRenderer)
	{
		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
}

CGameEngine * CGameEngine::GetSharedEngine()
{
	static CGameEngine *pEngine = nullptr;

	if (pEngine == nullptr)
	{
		pEngine = new CGameEngine();
	}

	return pEngine;
}

bool CGameEngine::Initialize(SwapChainPanel^ swapChainPanel)
{
	assert(swapChainPanel != nullptr && m_pRenderer != nullptr);
	bool bResult = false;

	bResult = m_pRenderer->Initialize(swapChainPanel);
	if (bResult == false)
	{
		return false;
	}

	return true;
}

void CGameEngine::UpdateForWindowSizeOrScaleChanged(const Windows::Foundation::Size * pNewLogicalSize, float fCompositionScaleX, float fCompositionScaleY)
{
	assert(m_pRenderer != nullptr);
	m_pRenderer->UpdateForWindowSizeOrScaleChanged(pNewLogicalSize, fCompositionScaleX, fCompositionScaleY);
}

void CGameEngine::Process() const
{
	assert(m_pRenderer != nullptr);
	m_pRenderer->BeginDraw();
	if (m_pGameWorld)
	{
		m_pRenderer->Draw(*m_pGameWorld);
	}
	m_pRenderer->EndDraw();
	m_pRenderer->Present();
}

void CGameEngine::AddWorld(CGameWorld * pWorld)
{
	assert(pWorld != nullptr);
	m_pGameWorld = pWorld;
}
