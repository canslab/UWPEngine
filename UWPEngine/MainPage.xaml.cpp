//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace UWPEngine;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::UI::Core;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();	
	
	swapChainPanel->SizeChanged += ref new Windows::UI::Xaml::SizeChangedEventHandler(this, &UWPEngine::MainPage::OnSwapChainPanelSizeChanged);
	swapChainPanel->CompositionScaleChanged += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Xaml::Controls::SwapChainPanel ^, Platform::Object ^>(this, &UWPEngine::MainPage::OnSwapChainCompositionScaleChanged);

	auto coreWindow = CoreWindow::GetForCurrentThread();
	coreWindow->PointerPressed += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(this, &UWPEngine::MainPage::OnPointerPressed);
	coreWindow->PointerMoved += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(this, &UWPEngine::MainPage::OnPointerMoved);
	coreWindow->PointerReleased += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::PointerEventArgs ^>(this, &UWPEngine::MainPage::OnPointerReleased);
	m_pEngine = CGameEngine::GetSharedEngine();

	auto bInitResult = m_pEngine->Initialize(swapChainPanel);
	if (bInitResult == false)
	{
		// 사용자에게 잘못되었다고 알려준다.
	}

	m_pObject = new CGameObject();
	bInitResult = m_pObject->Initialize("./Assets/box.obj");
	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}

	m_pCamera = new CCamera();
	m_pCamera->Initialize({ 0,0,-5,1 }, { 0,0,-4,1 }, { 0,1,0,0 });
	
	m_pEngine->AddObject(m_pObject);
	m_pEngine->AddCamera(m_pCamera);
	
	EventHandler<Object^>^ ev = ref new EventHandler<Object^>(this, &MainPage::OnUpdate);
	CompositionTarget::Rendering += ev;
}

void UWPEngine::MainPage::OnUpdate(Object ^ sender, Object ^ args)
{
	if (m_pEngine)
	{
		m_pEngine->Process();
	}
}

void UWPEngine::MainPage::OnSwapChainPanelSizeChanged(Platform::Object ^ sender, Windows::UI::Xaml::SizeChangedEventArgs ^ e)
{
	auto newSize = Windows::Foundation::Size(static_cast<float>(swapChainPanel->ActualWidth), static_cast<float>(swapChainPanel->ActualHeight));
	auto xScale = swapChainPanel->CompositionScaleX;
	auto yScale = swapChainPanel->CompositionScaleY;

	m_pEngine->UpdateForWindowSizeOrScaleChanged(&newSize, xScale, yScale);
}

void UWPEngine::MainPage::OnSwapChainCompositionScaleChanged(Windows::UI::Xaml::Controls::SwapChainPanel ^ sender, Platform::Object ^ args)
{
	auto newSize = Windows::Foundation::Size(static_cast<float>(swapChainPanel->ActualWidth), static_cast<float>(swapChainPanel->ActualHeight));
	auto xScale = swapChainPanel->CompositionScaleX;
	auto yScale = swapChainPanel->CompositionScaleY;

	m_pEngine->UpdateForWindowSizeOrScaleChanged(&newSize, xScale, yScale);
}

void UWPEngine::MainPage::OnPointerPressed(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
}

void UWPEngine::MainPage::OnPointerMoved(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
}

void UWPEngine::MainPage::
OnPointerReleased(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
}
