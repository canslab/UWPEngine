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
using namespace Windows::UI::Popups;

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
	m_pTimer = CGameTimer::GetSharedTimer();
	m_pTimer->Reset();

#ifdef ARM_ARCH
	auto statusBar = Windows::UI::ViewManagement::StatusBar::GetForCurrentView();
	statusBar->HideAsync();
#endif
	m_bPressed = false;
	
	auto bInitResult = m_pEngine->Initialize(swapChainPanel);
	if (bInitResult == false)
	{
		// 사용자에게 잘못되었다고 알려준다.
		MessageDialog ^msg = ref new MessageDialog("There is no DirectX 11 compatible graphic card...");
		concurrency::create_task(msg->ShowAsync()).then([](Windows::UI::Popups::IUICommand^ command)
		{
			// Windows Phone에서도 종료될지는 모르곘음..
			exit(1);
		});
		return;
	}
	
	// 오브젝트 1개를 만든다..
	m_pObject1 = new CGameObject();
	bInitResult = m_pObject1->Initialize("./Assets/BMW.obj");
	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}
	m_pObject2 = new CGameObject();
	bInitResult = m_pObject2->Initialize("./Assets/BMW.obj");
	m_pObject2->SetPositionW({ -50, 0, 0, 1 });

	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}

	m_pObject3 = new CGameObject();
	bInitResult = m_pObject3->Initialize("./Assets/BMW.obj");
	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}

	m_pObject4 = new CGameObject();
	bInitResult = m_pObject4->Initialize("./Assets/BMW.obj");
	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}

	m_pObject5 = new CGameObject();
	bInitResult = m_pObject5->Initialize("./Assets/arrow.obj");
	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}


	// 월드를 만든다
	m_pWorld = new CGameWorld();
	if (m_pWorld->Initialize())
	{
		// world 초기화 실패
	}

	// 월드에 오브젝트 두 개를 추가한다.
	m_pWorld->AddObject(m_pObject1);
	m_pWorld->AddObject(m_pObject2);
	m_pWorld->AddObject(m_pObject3);
	m_pWorld->AddObject(m_pObject4);
	m_pWorld->AddObject(m_pObject5);

	// 카메라 위치, 타깃, 업 벡터를 만들고, 월드 카메라에 설정한다.
	float cameraPosition[] = { 0,0,-5 };
	float cameraTargetPos[] = { 0,0,0 };
	float cameraUpVector[] = { 0,1,0 };

	auto& camera = m_pWorld->GetCamera();
	camera.SetPositionW(0, 0, -300);
	camera.SetTargetPositionW(0, 0, 0);
	camera.SetUpVectorW(0, 1, 0);

	// 엔진에 월드를 추가 한다.
	m_pEngine->SetWorld(m_pWorld);
	
	EventHandler<Object^>^ ev = ref new EventHandler<Object^>(this, &MainPage::OnUpdate);
	CompositionTarget::Rendering += ev;
}

void UWPEngine::MainPage::OnUpdate(Object ^ sender, Object ^ args)
{
	if (m_pEngine)
	{
		// Update Scene
		m_pTimer->Tick();
		auto totalTime = m_pTimer->GetTotalTime();
		//m_pWorld->GetCamera().MovePositionByOffsetZ(totalTime);

		m_pObject1->SetRotation({ 0, totalTime , totalTime });
		m_pWorld->Update(m_pObject1);

		// Render
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
	static float curXPos = 0.0f;
	auto curPosition = args->CurrentPoint->Position;
	if (curPosition.X < (swapChainPanel->ActualWidth / 2))
	{
		curXPos -= 0.1f;
	}
	else
	{
		curXPos += 0.1f;
	}
	m_bPressed = true;

	static float curZ = -5;
	auto& camera = m_pWorld->GetCamera();
	camera.MovePositionByOffsetZ(-1);
}

void UWPEngine::MainPage::OnPointerMoved(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
	

}

void UWPEngine::MainPage::
OnPointerReleased(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
}
