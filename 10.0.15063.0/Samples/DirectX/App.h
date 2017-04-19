#pragma once

#include "pch.h"
#include "Common\DeviceResources.h"
#include "DirectXMain.h"

namespace DirectX
{
	// Main entry point for our app. Connects the app with the Windows shell and handles application lifecycle events.
	class App : public winrt::implements<App, winrt::Windows::ApplicationModel::Core::IFrameworkView>
	{
	public:
		App();
		
		// IFrameworkView Methods.
		virtual void Initialize(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView);
		virtual void SetWindow(winrt::Windows::UI::Core::CoreWindow const& window);
		virtual void Load(winrt::hstring const& entryPoint);
		virtual void Run();
		virtual void Uninitialize();

	private:
		// Application lifecycle event handlers.
		void OnActivated(winrt::Windows::ApplicationModel::Core::CoreApplicationView const& applicationView, winrt::Windows::ApplicationModel::Activation::IActivatedEventArgs const& args);
		void OnSuspending(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::ApplicationModel::SuspendingEventArgs const& args);
		void OnResuming(winrt::Windows::Foundation::IInspectable  const& sender, winrt::Windows::Foundation::IInspectable const& args);

		// Window event handlers.
		void OnWindowSizeChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::WindowSizeChangedEventArgs const& args);
		void OnVisibilityChanged(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::VisibilityChangedEventArgs const& args);
		void OnWindowClosed(winrt::Windows::UI::Core::CoreWindow const& sender, winrt::Windows::UI::Core::CoreWindowEventArgs const& args);

		// DisplayInformation event handlers.
		void OnDpiChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);
		void OnOrientationChanged(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable  const& args);
		void OnDisplayContentsInvalidated(winrt::Windows::Graphics::Display::DisplayInformation const& sender, winrt::Windows::Foundation::IInspectable const& args);

		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::unique_ptr<DirectXMain> m_main;
		bool m_windowClosed;
		bool m_windowVisible;
	};
}

class Direct3DApplicationSource : public winrt::implements<Direct3DApplicationSource, winrt::Windows::ApplicationModel::Core::IFrameworkViewSource>
{
public:
	winrt::Windows::ApplicationModel::Core::IFrameworkView CreateView();

private:
	DirectX::App m_app;
};
