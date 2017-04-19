#include "pch.h"
#include "App.h"

using namespace DirectX;

using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::ApplicationModel::Activation;
using namespace winrt::Windows::UI::Core;
using namespace winrt::Windows::UI::Input;
using namespace winrt::Windows::System;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Graphics::Display;

// The main function is only used to initialize our IFrameworkView class.
int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	CoreApplication::Run(Direct3DApplicationSource());
}

IFrameworkView Direct3DApplicationSource::CreateView()
{
	return m_app;
}

App::App() :
	m_windowClosed(false),
	m_windowVisible(true)
{
}

// The first method called when the IFrameworkView is being created.
void App::Initialize(CoreApplicationView const& applicationView)
{
	// Register event handlers for app lifecycle. This example includes Activated, so that we
	// can make the CoreWindow active and start rendering on the window.
	applicationView.Activated({ this, &App::OnActivated });

	CoreApplication::Suspending({ this, &App::OnSuspending });

	CoreApplication::Resuming({ this, &App::OnResuming });

	// At this point we have access to the device. 
	// We can create the device-dependent resources.
	m_deviceResources = std::make_shared<DX::DeviceResources>();
}

// Called when the CoreWindow object is created (or re-created).
void App::SetWindow(CoreWindow const& window)
{
	window.SizeChanged({ this, &App::OnWindowSizeChanged });

	window.VisibilityChanged({ this, &App::OnVisibilityChanged });

	window.Closed({ this, &App::OnWindowClosed });

	DisplayInformation currentDisplayInformation = DisplayInformation::GetForCurrentView();

	currentDisplayInformation.DpiChanged({ this, &App::OnDpiChanged });

	currentDisplayInformation.OrientationChanged({ this, &App::OnOrientationChanged });

	DisplayInformation::DisplayContentsInvalidated({ this, &App::OnDisplayContentsInvalidated });

	m_deviceResources->SetWindow(window);
}

// Initializes scene resources, or loads a previously saved app state.
void App::Load(winrt::hstring const& entryPoint)
{
	if (m_main == nullptr)
	{
		m_main = std::unique_ptr<DirectXMain>(new DirectXMain(m_deviceResources));
	}
}

// This method is called after the window becomes active.
void App::Run()
{
	while (!m_windowClosed)
	{
		if (m_windowVisible)
		{
			CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			m_main->Update();

			if (m_main->Render())
			{
				m_deviceResources->Present();
			}
		}
		else
		{
			CoreWindow::GetForCurrentThread().Dispatcher().ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

// Required for IFrameworkView.
// Terminate events do not cause Uninitialize to be called. It will be called if your IFrameworkView
// class is torn down while the app is in the foreground.
void App::Uninitialize()
{
}

// Application lifecycle event handlers.

void App::OnActivated(CoreApplicationView const& applicationView, IActivatedEventArgs const& args)
{
	// Run() won't start until the CoreWindow is activated.
	CoreWindow::GetForCurrentThread().Activate();
}

void App::OnSuspending(IInspectable const& sender, SuspendingEventArgs const& args)
{
	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral deferral = args.SuspendingOperation().GetDeferral();

	std::async(std::launch::async, [this, deferral]()
	{
        m_deviceResources->Trim();

		// Insert your code here.

		deferral.Complete();
	});
}

void App::OnResuming(IInspectable const& sender, IInspectable const& args)
{
	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.

	// Insert your code here.
}

// Window event handlers.

void App::OnWindowSizeChanged(CoreWindow const& sender, WindowSizeChangedEventArgs const& args)
{
	m_deviceResources->SetLogicalSize(Size(sender.Bounds().Width, sender.Bounds().Height));
	m_main->CreateWindowSizeDependentResources();
}

void App::OnVisibilityChanged(CoreWindow const& sender, VisibilityChangedEventArgs const& args)
{
	m_windowVisible = args.Visible();
}

void App::OnWindowClosed(CoreWindow const& sender, CoreWindowEventArgs const& args)
{
	m_windowClosed = true;
}

// DisplayInformation event handlers.

void App::OnDpiChanged(DisplayInformation const& sender, IInspectable const& args)
{
	// Note: The value for LogicalDpi retrieved here may not match the effective DPI of the app
	// if it is being scaled for high resolution devices. Once the DPI is set on DeviceResources,
	// you should always retrieve it using the GetDpi method.
	// See DeviceResources.cpp for more details.
	m_deviceResources->SetDpi(sender.LogicalDpi());
	m_main->CreateWindowSizeDependentResources();
}

void App::OnOrientationChanged(DisplayInformation const& sender, IInspectable const& args)
{
	m_deviceResources->SetCurrentOrientation(sender.CurrentOrientation());
	m_main->CreateWindowSizeDependentResources();
}

void App::OnDisplayContentsInvalidated(DisplayInformation const& sender, IInspectable const& args)
{
	m_deviceResources->ValidateDevice();
}