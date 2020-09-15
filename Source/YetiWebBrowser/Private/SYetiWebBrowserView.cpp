// Copyright Epic Games, Inc. All Rights Reserved.

#include "SYetiWebBrowserView.h"
#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Containers/Ticker.h"
#include "YetiWebBrowserModule.h"
#include "Layout/WidgetPath.h"
#include "Framework/Application/MenuStack.h"
#include "Framework/Application/SlateApplication.h"
#include "IYetiWebBrowserDialog.h"
#include "IYetiWebBrowserWindow.h"
#include "YetiWebBrowserViewport.h"
#include "IYetiWebBrowserAdapter.h"

#if PLATFORM_ANDROID && USE_ANDROID_JNI
#	include "Android/AndroidWebBrowserWindow.h"
#elif PLATFORM_IOS
#	include "IOS/IOSPlatformWebBrowser.h"
#elif PLATFORM_PS4
#	include "PS4PlatformWebBrowser.h"
#elif WITH_CEF3
#	include "CEF/CEFWebBrowserWindow.h"
#else
#	define DUMMY_WEB_BROWSER 1
#endif

#define LOCTEXT_NAMESPACE "WebBrowser"

SYetiWebBrowserView::SYetiWebBrowserView()
{
}

SYetiWebBrowserView::~SYetiWebBrowserView()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->OnDocumentStateChanged().RemoveAll(this);
		BrowserWindow->OnNeedsRedraw().RemoveAll(this);
		BrowserWindow->OnTitleChanged().RemoveAll(this);
		BrowserWindow->OnUrlChanged().RemoveAll(this);
		BrowserWindow->OnToolTip().RemoveAll(this);
		BrowserWindow->OnShowPopup().RemoveAll(this);
		BrowserWindow->OnDismissPopup().RemoveAll(this);

		BrowserWindow->OnShowDialog().Unbind();
		BrowserWindow->OnDismissAllDialogs().Unbind();
		BrowserWindow->OnCreateWindow().Unbind();
		BrowserWindow->OnCloseWindow().Unbind();

		if (BrowserWindow->OnBeforeBrowse().IsBoundToObject(this))
		{
			BrowserWindow->OnBeforeBrowse().Unbind();
		}

		if (BrowserWindow->OnLoadUrl().IsBoundToObject(this))
		{
			BrowserWindow->OnLoadUrl().Unbind();
		}

		if (BrowserWindow->OnBeforePopup().IsBoundToObject(this))
		{
			BrowserWindow->OnBeforePopup().Unbind();
		}
	}

	TSharedPtr<SWindow> SlateParentWindow = SlateParentWindowPtr.Pin();
	if (SlateParentWindow.IsValid())
	{
		SlateParentWindow->GetOnWindowDeactivatedEvent().RemoveAll(this);
	}

	if (SlateParentWindow.IsValid())
	{
		SlateParentWindow->GetOnWindowActivatedEvent().RemoveAll(this);
	}
}

void SYetiWebBrowserView::Construct(const FArguments& InArgs, const TSharedPtr<IYetiWebBrowserWindow>& InWebBrowserWindow)
{
	OnLoadCompleted = InArgs._OnLoadCompleted;
	OnLoadError = InArgs._OnLoadError;
	OnLoadStarted = InArgs._OnLoadStarted;
	OnTitleChanged = InArgs._OnTitleChanged;
	OnUrlChanged = InArgs._OnUrlChanged;
	OnBeforeNavigation = InArgs._OnBeforeNavigation;
	OnLoadUrl = InArgs._OnLoadUrl;
	OnShowDialog = InArgs._OnShowDialog;
	OnDismissAllDialogs = InArgs._OnDismissAllDialogs;
	OnBeforePopup = InArgs._OnBeforePopup;
	OnCreateWindow = InArgs._OnCreateWindow;
	OnCloseWindow = InArgs._OnCloseWindow;
	AddressBarUrl = FText::FromString(InArgs._InitialURL);
	PopupMenuMethod = InArgs._PopupMenuMethod;
	OnUnhandledKeyDown = InArgs._OnUnhandledKeyDown;
	OnUnhandledKeyUp = InArgs._OnUnhandledKeyUp;
	OnUnhandledKeyChar = InArgs._OnUnhandledKeyChar;

	BrowserWindow = InWebBrowserWindow;
	if(!BrowserWindow.IsValid())
	{

		static bool AllowCEF = !FParse::Param(FCommandLine::Get(), TEXT("nocef"));
		bool bBrowserEnabled = true;
		GConfig->GetBool(TEXT("Browser"), TEXT("bEnabled"), bBrowserEnabled, GEngineIni);
		if (AllowCEF && bBrowserEnabled)
		{
			FCreateYetiBrowserWindowSettings Settings;
			Settings.InitialURL = InArgs._InitialURL;
			Settings.bUseTransparency = InArgs._SupportsTransparency;
			Settings.bThumbMouseButtonNavigation = InArgs._SupportsThumbMouseButtonNavigation;
			Settings.ContentsToLoad = InArgs._ContentsToLoad;
			Settings.bShowErrorMessage = InArgs._ShowErrorMessage;
			Settings.BackgroundColor = InArgs._BackgroundColor;
			Settings.BrowserFrameRate = InArgs._BrowserFrameRate;
			Settings.Context = InArgs._ContextSettings;
			Settings.AltRetryDomains = InArgs._AltRetryDomains;

			BrowserWindow = IYetiWebBrowserModule::Get().GetSingleton()->CreateBrowserWindow(Settings);
		}
	}

	SlateParentWindowPtr = InArgs._ParentWindow;

	if (BrowserWindow.IsValid())
	{
#ifndef DUMMY_WEB_BROWSER
		// The inner widget creation is handled by the WebBrowserWindow implementation.
		const auto& BrowserWidgetRef = static_cast<FWebBrowserWindow*>(BrowserWindow.Get())->CreateWidget();
		ChildSlot
		[
			BrowserWidgetRef
		];
		BrowserWidget = BrowserWidgetRef;
#endif

		if(OnCreateWindow.IsBound())
		{
			BrowserWindow->OnCreateWindow().BindSP(this, &SYetiWebBrowserView::HandleCreateWindow);
		}

		if(OnCloseWindow.IsBound())
		{
			BrowserWindow->OnCloseWindow().BindSP(this, &SYetiWebBrowserView::HandleCloseWindow);
		}

		BrowserWindow->OnDocumentStateChanged().AddSP(this, &SYetiWebBrowserView::HandleBrowserWindowDocumentStateChanged);
		BrowserWindow->OnNeedsRedraw().AddSP(this, &SYetiWebBrowserView::HandleBrowserWindowNeedsRedraw);
		BrowserWindow->OnTitleChanged().AddSP(this, &SYetiWebBrowserView::HandleTitleChanged);
		BrowserWindow->OnUrlChanged().AddSP(this, &SYetiWebBrowserView::HandleUrlChanged);
		BrowserWindow->OnToolTip().AddSP(this, &SYetiWebBrowserView::HandleToolTip);
		OnCreateToolTip = InArgs._OnCreateToolTip;

		if (!BrowserWindow->OnBeforeBrowse().IsBound())
		{
			BrowserWindow->OnBeforeBrowse().BindSP(this, &SYetiWebBrowserView::HandleBeforeNavigation);
		}
		else
		{
			check(!OnBeforeNavigation.IsBound());
		}

		if (!BrowserWindow->OnLoadUrl().IsBound())
		{
			BrowserWindow->OnLoadUrl().BindSP(this, &SYetiWebBrowserView::HandleLoadUrl);
		}
		else
		{
			check(!OnLoadUrl.IsBound());
		}

		if (!BrowserWindow->OnBeforePopup().IsBound())
		{
			BrowserWindow->OnBeforePopup().BindSP(this, &SYetiWebBrowserView::HandleBeforePopup);
		}
		else
		{
			check(!OnBeforePopup.IsBound());
		}

		if (!BrowserWindow->OnUnhandledKeyDown().IsBound())
		{
			BrowserWindow->OnUnhandledKeyDown().BindSP(this, &SYetiWebBrowserView::UnhandledKeyDown);
		}

		if (!BrowserWindow->OnUnhandledKeyUp().IsBound())
		{
			BrowserWindow->OnUnhandledKeyUp().BindSP(this, &SYetiWebBrowserView::UnhandledKeyUp);
		}

		if (!BrowserWindow->OnUnhandledKeyChar().IsBound())
		{
			BrowserWindow->OnUnhandledKeyChar().BindSP(this, &SYetiWebBrowserView::UnhandledKeyChar);
		}
		
		BrowserWindow->OnShowDialog().BindSP(this, &SYetiWebBrowserView::HandleShowDialog);
		BrowserWindow->OnDismissAllDialogs().BindSP(this, &SYetiWebBrowserView::HandleDismissAllDialogs);
		BrowserWindow->OnShowPopup().AddSP(this, &SYetiWebBrowserView::HandleShowPopup);
		BrowserWindow->OnDismissPopup().AddSP(this, &SYetiWebBrowserView::HandleDismissPopup);

		BrowserWindow->OnSuppressContextMenu().BindSP(this, &SYetiWebBrowserView::HandleSuppressContextMenu);


		OnSuppressContextMenu = InArgs._OnSuppressContextMenu;

		BrowserWindow->OnDragWindow().BindSP(this, &SYetiWebBrowserView::HandleDrag);
		OnDragWindow = InArgs._OnDragWindow;

		BrowserViewport = MakeShareable(new FYetiWebBrowserViewport(BrowserWindow));
#if WITH_CEF3
		BrowserWidget->SetViewportInterface(BrowserViewport.ToSharedRef());
#endif
		// If we could not obtain the parent window during widget construction, we'll defer and keep trying.
		SetupParentWindowHandlers();
	}
	else
	{
		OnLoadError.ExecuteIfBound();
	}
}

int32 SYetiWebBrowserView::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if (!SlateParentWindowPtr.IsValid())
	{
		SYetiWebBrowserView* MutableThis = const_cast<SYetiWebBrowserView*>(this);
		MutableThis->SetupParentWindowHandlers();
	}

	int32 Layer = SCompoundWidget::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
	
	// Cache a reference to our parent window, if we didn't already reference it.
	if (!SlateParentWindowPtr.IsValid())
	{
		SWindow* ParentWindow = OutDrawElements.GetPaintWindow();

		TSharedRef<SWindow> SlateParentWindowRef = StaticCastSharedRef<SWindow>(ParentWindow->AsShared());
		SlateParentWindowPtr = SlateParentWindowRef;
		if (BrowserWindow.IsValid())
		{
			BrowserWindow->SetParentWindow(SlateParentWindowRef);
		}
	}

	return Layer;
}

void SYetiWebBrowserView::HandleWindowDeactivated()
{
	if (BrowserViewport.IsValid())
	{
		BrowserViewport->OnFocusLost(FFocusEvent());
	}
}

void SYetiWebBrowserView::HandleWindowActivated()
{
	if (BrowserViewport.IsValid())
	{
		if (HasAnyUserFocusOrFocusedDescendants())
		{
			BrowserViewport->OnFocusReceived(FFocusEvent());
		}	
	}
}

void SYetiWebBrowserView::LoadURL(FString NewURL)
{
	AddressBarUrl = FText::FromString(NewURL);
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->LoadURL(NewURL);
	}
}

void SYetiWebBrowserView::LoadString(FString Contents, FString DummyURL)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->LoadString(Contents, DummyURL);
	}
}

void SYetiWebBrowserView::Reload()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->Reload();
	}
}

void SYetiWebBrowserView::StopLoad()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->StopLoad();
	}
}

FText SYetiWebBrowserView::GetTitleText() const
{
	if (BrowserWindow.IsValid())
	{
		return FText::FromString(BrowserWindow->GetTitle());
	}
	return LOCTEXT("InvalidWindow", "Browser Window is not valid/supported");
}

FString SYetiWebBrowserView::GetUrl() const
{
	if (BrowserWindow.IsValid())
	{
		return BrowserWindow->GetUrl();
	}

	return FString();
}

FText SYetiWebBrowserView::GetAddressBarUrlText() const
{
	if(BrowserWindow.IsValid())
	{
		return AddressBarUrl;
	}
	return FText::GetEmpty();
}

bool SYetiWebBrowserView::IsLoaded() const
{
	if (BrowserWindow.IsValid())
	{
		return (BrowserWindow->GetDocumentLoadingState() == EYetiWebBrowserDocumentState::Completed);
	}

	return false;
}

bool SYetiWebBrowserView::IsLoading() const
{
	if (BrowserWindow.IsValid())
	{
		return (BrowserWindow->GetDocumentLoadingState() == EYetiWebBrowserDocumentState::Loading);
	}

	return false;
}

bool SYetiWebBrowserView::CanGoBack() const
{
	if (BrowserWindow.IsValid())
	{
		return BrowserWindow->CanGoBack();
	}
	return false;
}

void SYetiWebBrowserView::GoBack()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->GoBack();
	}
}

bool SYetiWebBrowserView::CanGoForward() const
{
	if (BrowserWindow.IsValid())
	{
		return BrowserWindow->CanGoForward();
	}
	return false;
}

void SYetiWebBrowserView::GoForward()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->GoForward();
	}
}

bool SYetiWebBrowserView::IsInitialized() const
{
	return BrowserWindow.IsValid() &&  BrowserWindow->IsInitialized();
}

void SYetiWebBrowserView::SetupParentWindowHandlers()
{
	if (!SlateParentWindowPtr.IsValid())
	{
		SlateParentWindowPtr = FSlateApplication::Get().FindWidgetWindow(SharedThis(this));

		TSharedPtr<SWindow> SlateParentWindow = SlateParentWindowPtr.Pin();
		if (SlateParentWindow.IsValid() && BrowserWindow.IsValid())
		{
			if (!SlateParentWindow->GetOnWindowDeactivatedEvent().IsBoundToObject(this))
			{
				SlateParentWindow->GetOnWindowDeactivatedEvent().AddSP(this, &SYetiWebBrowserView::HandleWindowDeactivated);
			}

			if (!SlateParentWindow->GetOnWindowActivatedEvent().IsBoundToObject(this))
			{
				SlateParentWindow->GetOnWindowActivatedEvent().AddSP(this, &SYetiWebBrowserView::HandleWindowActivated);
			}

			BrowserWindow->SetParentWindow(SlateParentWindow);
		}
	}
}

void SYetiWebBrowserView::HandleBrowserWindowDocumentStateChanged(EYetiWebBrowserDocumentState NewState)
{
	switch (NewState)
	{
	case EYetiWebBrowserDocumentState::Completed:
		{
			if (BrowserWindow.IsValid())
			{
				for (auto Adapter : Adapters)
				{
					Adapter->ConnectTo(BrowserWindow.ToSharedRef());
				}
			}

			OnLoadCompleted.ExecuteIfBound();
		}
		break;

	case EYetiWebBrowserDocumentState::Error:
		OnLoadError.ExecuteIfBound();
		break;

	case EYetiWebBrowserDocumentState::Loading:
		OnLoadStarted.ExecuteIfBound();
		break;
	}
}

void SYetiWebBrowserView::HandleBrowserWindowNeedsRedraw()
{
	if (FSlateApplication::Get().IsSlateAsleep())
	{
		// Tell slate that the widget needs to wake up for one frame to get redrawn
		RegisterActiveTimer(0.f, FWidgetActiveTimerDelegate::CreateLambda([this](double InCurrentTime, float InDeltaTime) { return EActiveTimerReturnType::Stop; }));
	}
}

void SYetiWebBrowserView::HandleTitleChanged( FString NewTitle )
{
	const FText NewTitleText = FText::FromString(NewTitle);
	OnTitleChanged.ExecuteIfBound(NewTitleText);
}

void SYetiWebBrowserView::HandleUrlChanged( FString NewUrl )
{
	AddressBarUrl = FText::FromString(NewUrl);
	OnUrlChanged.ExecuteIfBound(AddressBarUrl);
}

void SYetiWebBrowserView::HandleToolTip(FString ToolTipText)
{
	if(ToolTipText.IsEmpty())
	{
		FSlateApplication::Get().CloseToolTip();
		SetToolTip(nullptr);
	}
	else if (OnCreateToolTip.IsBound())
	{
		SetToolTip(OnCreateToolTip.Execute(FText::FromString(ToolTipText)));
		FSlateApplication::Get().UpdateToolTip(true);
	}
	else
	{
		SetToolTipText(FText::FromString(ToolTipText));
		FSlateApplication::Get().UpdateToolTip(true);
	}
}

bool SYetiWebBrowserView::HandleBeforeNavigation(const FString& Url, const FYetiWebNavigationRequest& Request)
{
	if(OnBeforeNavigation.IsBound())
	{
		return OnBeforeNavigation.Execute(Url, Request);
	}
	return false;
}

bool SYetiWebBrowserView::HandleLoadUrl(const FString& Method, const FString& Url, FString& OutResponse)
{
	if(OnLoadUrl.IsBound())
	{
		return OnLoadUrl.Execute(Method, Url, OutResponse);
	}
	return false;
}

EYetiWebBrowserDialogEventResponse SYetiWebBrowserView::HandleShowDialog(const TWeakPtr<IYetiWebBrowserDialog>& DialogParams)
{
	if(OnShowDialog.IsBound())
	{
		return OnShowDialog.Execute(DialogParams);
	}
	return EYetiWebBrowserDialogEventResponse::Unhandled;
}

void SYetiWebBrowserView::HandleDismissAllDialogs()
{
	OnDismissAllDialogs.ExecuteIfBound();
}


bool SYetiWebBrowserView::HandleBeforePopup(FString URL, FString Target)
{
	if (OnBeforePopup.IsBound())
	{
		return OnBeforePopup.Execute(URL, Target);
	}

	return false;
}

void SYetiWebBrowserView::ExecuteJavascript(const FString& ScriptText)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->ExecuteJavascript(ScriptText);
	}
}

void SYetiWebBrowserView::GetSource(TFunction<void (const FString&)> Callback) const
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->GetSource(Callback);
	}
}


bool SYetiWebBrowserView::HandleCreateWindow(const TWeakPtr<IYetiWebBrowserWindow>& NewBrowserWindow, const TWeakPtr<IYetiWebBrowserPopupFeatures>& PopupFeatures)
{
	if(OnCreateWindow.IsBound())
	{
		return OnCreateWindow.Execute(NewBrowserWindow, PopupFeatures);
	}
	return false;
}

bool SYetiWebBrowserView::HandleCloseWindow(const TWeakPtr<IYetiWebBrowserWindow>& NewBrowserWindow)
{
	if(OnCloseWindow.IsBound())
	{
		return OnCloseWindow.Execute(NewBrowserWindow);
	}
	return false;
}

void SYetiWebBrowserView::BindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->BindUObject(Name, Object, bIsPermanent);
	}
}

void SYetiWebBrowserView::UnbindUObject(const FString& Name, UObject* Object, bool bIsPermanent)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->UnbindUObject(Name, Object, bIsPermanent);
	}
}

void SYetiWebBrowserView::BindAdapter(const TSharedRef<IYetiWebBrowserAdapter>& Adapter)
{
	Adapters.Add(Adapter);
	if (BrowserWindow.IsValid())
	{
		Adapter->ConnectTo(BrowserWindow.ToSharedRef());
	}
}

void SYetiWebBrowserView::UnbindAdapter(const TSharedRef<IYetiWebBrowserAdapter>& Adapter)
{
	Adapters.Remove(Adapter);
	if (BrowserWindow.IsValid())
	{
		Adapter->DisconnectFrom(BrowserWindow.ToSharedRef());
	}
}

void SYetiWebBrowserView::BindInputMethodSystem(ITextInputMethodSystem* TextInputMethodSystem)
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->BindInputMethodSystem(TextInputMethodSystem);
	}
}

void SYetiWebBrowserView::UnbindInputMethodSystem()
{
	if (BrowserWindow.IsValid())
	{
		BrowserWindow->UnbindInputMethodSystem();
	}
}

void SYetiWebBrowserView::HandleShowPopup(const FIntRect& PopupSize)
{
	check(!PopupMenuPtr.IsValid())

	TSharedPtr<SViewport> MenuContent;
	SAssignNew(MenuContent, SViewport)
				.ViewportSize(PopupSize.Size())
				.EnableGammaCorrection(false)
				.EnableBlending(false)
				.IgnoreTextureAlpha(true)
				.Visibility(EVisibility::Visible);
	MenuViewport = MakeShareable(new FYetiWebBrowserViewport(BrowserWindow, true));
	MenuContent->SetViewportInterface(MenuViewport.ToSharedRef());
	FWidgetPath WidgetPath;
	FSlateApplication::Get().GeneratePathToWidgetUnchecked(SharedThis(this), WidgetPath);
	if (WidgetPath.IsValid())
	{
		TSharedRef< SWidget > MenuContentRef = MenuContent.ToSharedRef();
		const FGeometry& BrowserGeometry = WidgetPath.Widgets.Last().Geometry;
		const FVector2D NewPosition = BrowserGeometry.LocalToAbsolute(PopupSize.Min);


		// Open the pop-up. The popup method will be queried from the widget path passed in.
		TSharedPtr<IMenu> NewMenu = FSlateApplication::Get().PushMenu(SharedThis(this), WidgetPath, MenuContentRef, NewPosition, FPopupTransitionEffect( FPopupTransitionEffect::ComboButton ), false);
		NewMenu->GetOnMenuDismissed().AddSP(this, &SYetiWebBrowserView::HandleMenuDismissed);
		PopupMenuPtr = NewMenu;
	}

}

void SYetiWebBrowserView::HandleMenuDismissed(TSharedRef<IMenu>)
{
	PopupMenuPtr.Reset();
}

void SYetiWebBrowserView::HandleDismissPopup()
{
	if (PopupMenuPtr.IsValid())
	{
		PopupMenuPtr.Pin()->Dismiss();
		FSlateApplication::Get().SetKeyboardFocus(SharedThis(this), EFocusCause::SetDirectly);
	}
}

bool SYetiWebBrowserView::HandleSuppressContextMenu()
{
	if (OnSuppressContextMenu.IsBound())
	{
		return OnSuppressContextMenu.Execute();
	}

	return false;
}

bool SYetiWebBrowserView::HandleDrag(const FPointerEvent& MouseEvent)
{
	if (OnDragWindow.IsBound())
	{
		return OnDragWindow.Execute(MouseEvent);
	}
	return false;
}

bool SYetiWebBrowserView::UnhandledKeyDown(const FKeyEvent& KeyEvent)
{
	if (OnUnhandledKeyDown.IsBound())
	{
		return OnUnhandledKeyDown.Execute(KeyEvent);
	}
	return false;
}

bool SYetiWebBrowserView::UnhandledKeyUp(const FKeyEvent& KeyEvent)
{
	if (OnUnhandledKeyUp.IsBound())
	{
		return OnUnhandledKeyUp.Execute(KeyEvent);
	}
	return false;
}

bool SYetiWebBrowserView::UnhandledKeyChar(const FCharacterEvent& CharacterEvent)
{
	if (OnUnhandledKeyChar.IsBound())
	{
		return OnUnhandledKeyChar.Execute(CharacterEvent);
	}
	return false;
}


#undef LOCTEXT_NAMESPACE
