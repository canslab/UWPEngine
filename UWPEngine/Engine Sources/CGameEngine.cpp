#include "pch.h"
#include "CGameEngine.h"
#include <fstream>

using namespace Windows::UI::Xaml::Controls;
using namespace std;

// think of m_pCameraList as vector<Camera*>

CGameEngine::CGameEngine() :
	m_pRenderer(new CD3DRenderer()),
	m_pCameraList(new vector<CCamera*>()),
	m_pObjectList(new vector<CGameObject*>())
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

	if (m_pCameraList)
	{
		for (auto cameraPointer : *m_pCameraList)
		{
			if (cameraPointer)
			{
				delete cameraPointer;
			}
		}
	}

	if (m_pObjectList)
	{
		for (auto objectPointer : *m_pObjectList)
		{
			if (objectPointer)
			{
				delete objectPointer;
			}
		}
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

	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMStoreFloat4x4(&viewMatrix ,m_pCameraList->at(0)->GetViewMatrix());

	m_pRenderer->Draw(*(m_pObjectList->at(0)), viewMatrix);
	m_pRenderer->EndDraw();
	m_pRenderer->Present();
}

void CGameEngine::AddObject(CGameObject* object)
{
	assert(object != nullptr && object->IsInitalized() == true);
	m_pObjectList->push_back(object);
}

void CGameEngine::AddCamera(CCamera * camera)
{
	assert(camera != nullptr);
	m_pCameraList->push_back(camera);
}

