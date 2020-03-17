// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_WebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "WebBrowserWidget/Public/WebBrowser.h"
#include "WebBrowser/Public/SWebBrowser.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "YetiOS_Types.h"
#include <regex>

static const FString BROWSER_IDENTIFIER_FAILSAFE = "browser";

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_WebBrowser::UYetiOS_WebBrowser(const FObjectInitializer& ObjectInitializer)
{
	InitialURL = FString("https://google.com");	
	bSupportsTransparency = false;
	bShowWhitelistOnly = false;
	bOnlyHTTPS = true;
	bEnableHistory = true;
	bSupportBrowserURLs = true;
	BrowserIdentifier = "yetibrowser";
}

const bool UYetiOS_WebBrowser::LoadURL(const FText& URL)
{
	if (URL.IsEmptyOrWhitespace() == false)
	{
		FString NewURL = URL.ToString();

		if (bShowWhitelistOnly)
		{
			bool bDenyAccess = true;
			for (const FString It : WhitelistWebsites)
			{
				if (It.Contains(NewURL))
				{
					bDenyAccess = false;
					break;
				}
			}

			if (bDenyAccess)
			{
				OnAccessDenied.Broadcast();
				return false;
			}
		}

		const bool bIsBrowserURL = Internal_IsBrowserURL(NewURL);
		if (bIsBrowserURL)
		{
			OnLoadBrowserURL.Broadcast();
			return true;
		}

		if (bOnlyHTTPS && NewURL.StartsWith("http://"))
		{
			NewURL.ReplaceInline(TEXT("http://"), TEXT("https://"));
		}

		const bool bIsValidURL = Internal_IsURLValid(NewURL);
		if (bIsValidURL)
		{
			if (NewURL.StartsWith("http") == false)
			{
				NewURL = "https://" + NewURL;
			}
		}
		else
		{
			NewURL = FString::Printf(TEXT("https://www.google.com/search?q=%s"), *URL.ToString().Replace(TEXT(" "), TEXT("+")));
		}

		if (WebBrowserWidget.IsValid())
		{
			WebBrowserWidget->LoadURL(NewURL);
			return true;
		}
	}

	return false;
}

void UYetiOS_WebBrowser::LoadString(FString Contents, FString DummyURL)
{
	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->LoadString(Contents, DummyURL);
	}
}

void UYetiOS_WebBrowser::ExecuteJavascript(const FString& ScriptText)
{
	if (WebBrowserWidget.IsValid())
	{
		WebBrowserWidget->ExecuteJavascript(ScriptText);
	}
}

const FText UYetiOS_WebBrowser::GetTitleText() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetTitleText();
	}

	return FText::GetEmpty();
}

const FString UYetiOS_WebBrowser::GetUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetUrl();
	}

	return FString();
}

const FText UYetiOS_WebBrowser::GetAddressbarUrl() const
{
	if (WebBrowserWidget.IsValid())
	{
		return WebBrowserWidget->GetAddressBarUrlText();
	}

	return FText::GetEmpty();
}

const TArray<FString> UYetiOS_WebBrowser::GetBrowserURLs() const
{
	TArray<FString> ReturnArray;

	for (uint8 i = 0; i < (uint8)EBrowserURL::EMAX; i++)
	{
		EBrowserURL CurrentBrowserURL = static_cast<EBrowserURL>(i);
		const FString BrowserURL = FString::Printf(TEXT("%s%s"), *GetBrowserProtocolLink(), *ENUM_TO_STRING(EBrowserURL, CurrentBrowserURL).ToLower());
		ReturnArray.Add(BrowserURL);
	}

	return ReturnArray;
}

const bool UYetiOS_WebBrowser::GetBrowserUrlFromString(const FString& InString, EBrowserURL& OutResult)
{
	return STRING_TO_ENUM(EBrowserURL, InString, OutResult);
}

const FString UYetiOS_WebBrowser::GetBrowserProtocolLink() const
{
	const FString Identifier = BrowserIdentifier.IsEmpty() ? BROWSER_IDENTIFIER_FAILSAFE : BrowserIdentifier;
	return FString::Printf(TEXT("%s://"), *Identifier);
}

const TArray<FWebHistory> UYetiOS_WebBrowser::GetHistory() const
{
	return History;
}

void UYetiOS_WebBrowser::AssignDelegates()
{
	if (ReloadButton)
	{
		if (ReloadButton->OnClicked.IsBound())
		{
			ReloadButton->OnClicked.Clear();
		}

		ReloadButton->OnClicked.AddDynamic(this, &UYetiOS_WebBrowser::ReloadWebPage);
	}

	if (BackButton && ForwardButton)
	{
		if (BackButton->OnClicked.IsBound())
		{
			BackButton->OnClicked.Clear();
		}

		BackButton->OnClicked.AddDynamic(this, &UYetiOS_WebBrowser::Internal_GoBack);

		if (ForwardButton->OnClicked.IsBound())
		{
			ForwardButton->OnClicked.Clear();
		}

		ForwardButton->OnClicked.AddDynamic(this, &UYetiOS_WebBrowser::Internal_GoForward);
	}
}

TSharedRef<SWidget> UYetiOS_WebBrowser::RebuildWidget()
{
	if (IsDesignTime())
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Yeti OS Web Browser", "Yeti OS Web Browser"))
			];
	}
	
	WebBrowserWidget = SNew(SWebBrowser)
		.InitialURL(InitialURL)
		.ShowControls(false)
		.SupportsTransparency(bSupportsTransparency)
		.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
		.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup))
		.OnLoadStarted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadStart))
		.OnLoadCompleted(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadComplete))
		.OnLoadError(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnLoadError));

	return WebBrowserWidget.ToSharedRef();
}

void UYetiOS_WebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	WebBrowserWidget.Reset();	
}

#if WITH_EDITOR
const FText UYetiOS_WebBrowser::GetPaletteCategory()
{
	return LOCTEXT("YetiOS", "Yeti OS");
}
#endif

void UYetiOS_WebBrowser::ReloadWebPage()
{
	if (WebBrowserWidget.IsValid())
	{
		if (WebBrowserWidget->IsLoading())
		{
			WebBrowserWidget->StopLoad();
		}
		else
		{
			WebBrowserWidget->Reload();
		}
	}
}

void UYetiOS_WebBrowser::HandleOnUrlChanged(const FText& Text)
{
	OnUrlChanged.Broadcast(Text);
}

void UYetiOS_WebBrowser::HandleOnLoadStart()
{
	OnLoadStarted.Broadcast();
}

void UYetiOS_WebBrowser::HandleOnLoadComplete()
{
	if (WebBrowserWidget.IsValid())
	{
		if (Addressbar)
		{
			Addressbar->SetText(WebBrowserWidget->GetAddressBarUrlText());
		}

		if (bEnableHistory)
		{
			History.Add(FWebHistory(WebBrowserWidget->GetTitleText(), WebBrowserWidget->GetUrl()));
		}

		if (BackButton && ForwardButton)
		{
			BackButton->SetIsEnabled(WebBrowserWidget->CanGoBack());
			ForwardButton->SetIsEnabled(WebBrowserWidget->CanGoForward());
		}
	}

	OnLoadCompleted.Broadcast();
}

void UYetiOS_WebBrowser::HandleOnLoadError()
{
	OnLoadError.Broadcast();
}

bool UYetiOS_WebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
{
	if (OnBeforePopup.IsBound())
	{
		if (IsInGameThread())
		{
			OnBeforePopup.Broadcast(URL, Frame);
		}
		else
		{
			// Retry on the GameThread.
			TWeakObjectPtr<UYetiOS_WebBrowser> WeakThis = this;
			FFunctionGraphTask::CreateAndDispatchWhenReady([WeakThis, URL, Frame]()
			{
				if (WeakThis.IsValid())
				{
					WeakThis->HandleOnBeforePopup(URL, Frame);
				}
			}, TStatId(), nullptr, ENamedThreads::GameThread);
		}

		return true;
	}

	return false;
}

void UYetiOS_WebBrowser::Internal_GoBack()
{
	if (WebBrowserWidget.IsValid())
	{
		if (ensureMsgf(WebBrowserWidget->CanGoBack(), TEXT("Tried to navigate backward but widget does not allow navigating backward.")))
		{
			WebBrowserWidget->GoBack();
		}
	}
}

void UYetiOS_WebBrowser::Internal_GoForward()
{
	if (WebBrowserWidget.IsValid())
	{
		if (ensureMsgf(WebBrowserWidget->CanGoForward(), TEXT("Tried to navigate forward but widget does not allow navigating forward.")))
		{
			WebBrowserWidget->GoForward();
		}
	}
}

const bool UYetiOS_WebBrowser::Internal_IsURLValid(const FString& InURL) const
{
	static const std::regex RegPattern("^((http|https):\\/\\/)?(www.)?(?!.*(http|https|www.))[a-zA-Z0-9_-]+(\\.[a-zA-Z]+)+((\\/)[\\w#]+)*(\\/\\w+\\?[a-zA-Z0-9_]+=\\w+(&[a-zA-Z0-9_]+=\\w+)*)?$");
	return std::regex_match(TCHAR_TO_UTF8(*InURL), RegPattern);
}

const bool UYetiOS_WebBrowser::Internal_IsBrowserURL(const FString& InURL) const
{
	return bSupportBrowserURLs && InURL.StartsWith(GetBrowserProtocolLink());
}

#undef LOCTEXT_NAMESPACE
