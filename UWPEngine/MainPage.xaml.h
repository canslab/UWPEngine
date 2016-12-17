//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace UWPEngine
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

		void OnUpdate(Object^ sender, Object^ args);
		void OnSwapChainPanelSizeChanged(Platform::Object^ sender, Windows::UI::Xaml::SizeChangedEventArgs^ e);
		void OnSwapChainCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel ^ sender, Platform::Object^ args);

		void OnPointerPressed(Windows::UI::Core::CoreWindow ^coreWindow, Windows::UI::Core::PointerEventArgs ^args);
		void OnPointerMoved(Windows::UI::Core::CoreWindow ^coreWindow, Windows::UI::Core::PointerEventArgs ^args);
		void OnPointerReleased(Windows::UI::Core::CoreWindow ^coreWindow, Windows::UI::Core::PointerEventArgs ^args);
	
	private:
		CGameEngine *m_pEngine;
		CGameTimer *m_pTimer;
		CGameWorld *m_pWorld;
		CGameObject *m_pObject1;
		CGameObject *m_pObject2;
		CGameObject *m_pObject3;
		CGameObject *m_pObject4;
		CGameObject *m_pObject5;

		Windows::Foundation::Point m_pressedPoint;
		bool m_bPressed;
		void OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args);
	};
}
