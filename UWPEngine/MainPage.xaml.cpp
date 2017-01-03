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
	coreWindow->KeyDown += ref new Windows::Foundation::TypedEventHandler<Windows::UI::Core::CoreWindow ^, Windows::UI::Core::KeyEventArgs ^>(this, &UWPEngine::MainPage::OnKeyDown);

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
	bInitResult = m_pObject1->Initialize("./Assets/teapot.obj");
	if (bInitResult)
	{
		// Asset에서 불러오는 데 실패했다는 이야기.
	}
	m_pObject2 = new CGameObject();
	bInitResult = m_pObject2->Initialize("./Assets/BMW.obj");
	m_pObject2->SetScale(0.2f, 0.2f, 0.2f);
	m_pObject2->SetPositionW(-50, 0, 0);

	m_pObject3 = new CGameObject();
	bInitResult = m_pObject3->Initialize("./Assets/teapot.obj");
	m_pObject3->SetPositionW(+50, 0, 0);

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
	/*m_pWorld->AddObject(m_pObject5);*/

	// 카메라 위치, 타깃, 업 벡터를 만들고, 월드 카메라에 설정한다.
	float cameraPosition[] = { 0,0,-5 };
	float cameraTargetPos[] = { 0,0,0 };
	float cameraUpVector[] = { 0,1,0 };

	auto& camera = m_pWorld->GetCamera();
	camera.SetPositionW(0, 0, -100);
	camera.SetTargetPositionW(0, 0, 0);
	camera.SetUpVectorW(0, 1, 0);

	// 엔진에 월드를 추가 한다.
	m_pEngine->SetWorld(m_pWorld);

	EventHandler<Object^>^ ev = ref new EventHandler<Object^>(this, &MainPage::OnUpdate);
	CompositionTarget::Rendering += ev;
}

void UWPEngine::MainPage::OnUpdate(Object ^ sender, Object ^ args)
{
	assert(m_pEngine != nullptr);
	wchar_t buffer[100];

	// Update Scene
	m_pTimer->Tick();

	auto deltaTime = m_pTimer->GetDeltaTime();
	auto totalTime = m_pTimer->GetTotalTime();

	m_pObject1->SetRotation(0, totalTime, totalTime);
	m_pObject2->SetRotation(0, 2 * totalTime, 0);
	m_pWorld->Update(m_pObject1);
	m_pWorld->Update(m_pObject2);


	// Render
	swprintf_s(buffer, L"Demo Program FPS = %.2f", 1 / deltaTime);
	m_pEngine->UpdateContents();
	m_pEngine->DrawText(0, 0, 0.6, 0.05, L"Arial", buffer);
	m_pEngine->DrawText(0, 0.95, 0.4, 1, L"Segoe UI Light", L"Score = ");
	m_pEngine->Present();
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
	m_bPressed = true;
	m_pressedPoint = args->CurrentPoint->Position;
}

void UWPEngine::MainPage::OnPointerMoved(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
	if (m_bPressed)
	{
		auto& camera = m_pWorld->GetCamera();
		auto nowPoint = args->CurrentPoint->Position;
		auto xDiff = nowPoint.X - m_pressedPoint.X;
		auto yDiff = nowPoint.Y - m_pressedPoint.Y;

		camera.MovePositionBy(xDiff / 20, 0, 0);
		camera.MovePositionBy(0, -yDiff / 20, 0);
		camera.MoveTargetPositionByOffset(xDiff / 20, 0, 0);
		camera.MoveTargetPositionByOffset(0, -yDiff / 20, 0);

		m_pressedPoint = nowPoint;
	}
}

void UWPEngine::MainPage::OnPointerReleased(Windows::UI::Core::CoreWindow ^ coreWindow, Windows::UI::Core::PointerEventArgs ^ args)
{
	m_bPressed = false;
}

void UWPEngine::MainPage::OnKeyDown(Windows::UI::Core::CoreWindow ^sender, Windows::UI::Core::KeyEventArgs ^args)
{
	auto pressedKey = args->VirtualKey;
	CCamera& camera = m_pWorld->GetCamera();

	if (pressedKey == Windows::System::VirtualKey::W)
	{
		camera.MovePositionByOffsetZ(+1);
		camera.MoveTargetPositionByOffset(0, 0, +1);
	}
	else if (pressedKey == Windows::System::VirtualKey::A)
	{
		camera.MovePositionByOffsetX(-1);
		camera.MoveTargetPositionByOffset(-1, 0, 0);
	}
	else if (pressedKey == Windows::System::VirtualKey::S)
	{
		camera.MovePositionByOffsetZ(-1);
		camera.MoveTargetPositionByOffset(0, 0, -1);
	}
	else if (pressedKey == Windows::System::VirtualKey::D)
	{
		camera.MovePositionByOffsetX(+1);
		camera.MoveTargetPositionByOffset(+1, 0, 0);
	}

	int c = 30;
	c = 45;
}