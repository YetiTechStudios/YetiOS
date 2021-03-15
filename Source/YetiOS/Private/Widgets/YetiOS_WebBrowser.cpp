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
#include "WebBrowserModule.h"
#include "IWebBrowserCookieManager.h"
#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"
#include <regex>

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsWebBrowser, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsWebBrowser, Log, TEXT("%s"), *FString(Param1))
#define printlog_vv(Param1)				UE_LOG(LogYetiOsWebBrowser, VeryVerbose, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsWebBrowser, Error, TEXT("%s"), *FString(Param1))

static const FString BROWSER_IDENTIFIER_FAILSAFE = "browser";
static const FString DEFAULT_URL = "about:blank";

typedef struct FWebBrowserCookie FCookie;

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_WebBrowser::UYetiOS_WebBrowser(const FObjectInitializer& ObjectInitializer)
{
	InitialURL = FString("https://google.com");	
	bSupportsTransparency = false;
	bShowWhitelistOnly = false;
	bAllowURLMasking = false;
	bUrlMaskIsPersistent = true;
	bOnlyHTTPS = true;
	bEnableHistory = true;
	bSupportBrowserURLs = true;
	BrowserIdentifier = "yetibrowser";
}

const bool UYetiOS_WebBrowser::DeleteWebBrowserCache(const bool bPrintToLog /*= true*/)
{
	bool bSuccess = false;
	const FString CacheDirPath = FPaths::Combine(FPaths::ProjectSavedDir(), *FString("webcache"));
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (PlatformFile.DirectoryExists(*CacheDirPath))
	{
		bSuccess = PlatformFile.DeleteDirectoryRecursively(*CacheDirPath);
		if (bPrintToLog)
		{
			if (bSuccess)
			{
				printlog(FString::Printf(TEXT("Delete webcache folder: %s"), *CacheDirPath));
			}
			else
			{
				printlog_error(FString::Printf(TEXT("Failed to delete webcache folder: %s"), *CacheDirPath));
			}
		}
	}
	else
	{
		printlog_error(FString::Printf(TEXT("webcache does not exist: %s"), *CacheDirPath));
	}

	return bSuccess;
}

const bool UYetiOS_WebBrowser::LoadURL(const FText& URL)
{
	if (URL.IsEmptyOrWhitespace() == false)
	{
		FString NewURL = URL.ToString();
		LastLoadedURL = NewURL;

		if (bShowWhitelistOnly)
		{
			bool bDenyAccess = true;
			for (const FString& It : WhitelistWebsites)
			{
				if (NewURL.Contains(It))
				{
					bDenyAccess = false;
					break;
				}
			}

			if (bDenyAccess)
			{
				OnAccessDenied.Broadcast();
				Addressbar->SetText(URL);
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
			FString MaskedURL = "";
			if (Internal_FindMaskedURL(URL, MaskedURL))
			{
				NewURL = MaskedURL;
			}

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

void UYetiOS_WebBrowser::AddBookmark(const FBrowserBookmark& InBookmark)
{
	Bookmarks.Add(InBookmark);
}

void UYetiOS_WebBrowser::RemoveBookmark(const FBrowserBookmark& InBookmark)
{
	Bookmarks.RemoveSingle(InBookmark);
}

void UYetiOS_WebBrowser::SetCookie(const FString& URL, const FBrowserCookie& InCookie, FOnCookieSetComplete Delegate)
{
	IWebBrowserSingleton* BrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	TSharedPtr<class IWebBrowserCookieManager> BrowserCookie = BrowserSingleton->GetCookieManager();
	
	FCookie NewCookie;
	NewCookie.bHasExpires = InCookie.bExpires;
	NewCookie.bHttpOnly = InCookie.bHttpRequestOnly;
	NewCookie.bSecure = InCookie.bHttpsRequestsOnly;
	NewCookie.Domain = InCookie.Domain;
	NewCookie.Expires = InCookie.ExpireTime;
	NewCookie.Name = InCookie.Name;
	NewCookie.Path = InCookie.Path;
	NewCookie.Value = InCookie.Value;

	TFunction<void(bool)> OnSetFunc = [&](bool bSuccess)
	{ 
		printlog_vv(FString::Printf(TEXT("Set cookie (%s) %s on %s."), *NewCookie.Name, bSuccess ? *FString("success") : *FString("failed"), URL == "" ? *FString("all urls") : *URL));
		Delegate.ExecuteIfBound(bSuccess); 
	};

	BrowserCookie->SetCookie(URL, NewCookie, OnSetFunc);
}

void UYetiOS_WebBrowser::SetCookieForAll(const FBrowserCookie& InCookie, FOnCookieSetComplete Delegate)
{
	SetCookie("", InCookie, Delegate);
}

void UYetiOS_WebBrowser::DeleteCookie(const FString& URL, const FString& CookieName, FOnCookieDeleteComplete Delegate)
{
	IWebBrowserSingleton* BrowserSingleton = IWebBrowserModule::Get().GetSingleton();
	TSharedPtr<class IWebBrowserCookieManager> BrowserCookie = BrowserSingleton->GetCookieManager();

	TFunction<void(int)> OnDeleteFunc = [&](int Total)
	{
		printlog_vv(FString::Printf(TEXT("Delete Cookie [%s]: Total (%i). URL(s): ."), *CookieName, Total, URL == "" ? *FString("all urls") : *URL));
		Delegate.ExecuteIfBound(Total);
	};
	BrowserCookie->DeleteCookies(URL, CookieName, OnDeleteFunc);
}

void UYetiOS_WebBrowser::DeleteAllCookies(FOnCookieDeleteComplete Delegate)
{
	DeleteCookie("","", Delegate);
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

const FText UYetiOS_WebBrowser::GetCleanDomainName(const FText& InURL)
{	
	const std::string SearchString = TCHAR_TO_UTF8(*InURL.ToString());
	static const std::regex RegexPattern("(www)?[a-z0-9]+([\\-\\.]{1}[a-z0-9]+)*\\.[a-z]{2,5}(:[0-9]{1,5})?");
	std::smatch RegexMatch;
	std::regex_search(SearchString, RegexMatch, RegexPattern);
		
	const FString ReturnString = RegexMatch.str().c_str();

	return FText::FromString(ReturnString);
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

void UYetiOS_WebBrowser::InitializeWebBrowser(const FString InOverrideURL /*= ""*/)
{
	if (ReloadButton)
	{
		if (ReloadButton->OnClicked.IsBound())
		{
			ReloadButton->OnClicked.Clear();
		}

		ReloadButton->OnClicked.AddDynamic(this, &UYetiOS_WebBrowser::ReloadWebPage);
	}
	else
	{
		printlog_error("Reload button was not found. Make sure you have a button in UMG designer with variable name set to ReloadButton and Is Variable is true.");
	}

	if (BackButton)
	{
		if (BackButton->OnClicked.IsBound())
		{
			BackButton->OnClicked.Clear();
		}

		BackButton->OnClicked.AddDynamic(this, &UYetiOS_WebBrowser::Internal_GoBack);
	}
	else
	{
		printlog_error("Back button was not found. Make sure you have a button in UMG designer with variable name set to BackButton and Is Variable is true.");
	}

	if (ForwardButton)
	{
		if (ForwardButton->OnClicked.IsBound())
		{
			ForwardButton->OnClicked.Clear();
		}

		ForwardButton->OnClicked.AddDynamic(this, &UYetiOS_WebBrowser::Internal_GoForward);
	}
	else
	{
		printlog_error("Forward button was not found. Make sure you have a button in UMG designer with variable name set to ForwardButton and Is Variable is true.");
	}

	if (Addressbar == nullptr)
	{
		printlog_error("Address bar was not found. Make sure you have an Editable Textbox in UMG designer with variable name set to Addressbar and Is Variable is true.");
	}

	LoadURL(FText::FromString(InOverrideURL.IsEmpty() ? InitialURL : InOverrideURL));
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
		.InitialURL(DEFAULT_URL)
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
	if (bShowWhitelistOnly)
	{
		FString NewURL = Text.ToString();
		if (NewURL.Contains(InitialURL) || NewURL.Equals(DEFAULT_URL, ESearchCase::IgnoreCase))
		{
			return;
		}

		for (const FString& It : WhitelistWebsites)
		{
			if (NewURL.Contains(It))
			{
				return;
			}
		}

		OnAccessDenied.Broadcast();
		Addressbar->SetText(Text);
	}
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
			FText AddressbarURL = WebBrowserWidget->GetAddressBarUrlText();

			FString MaskedURL = "";
			if (Internal_FindMaskedURL(FText::FromString(LastLoadedURL), MaskedURL))
			{
				FString AddressbarUrlString = AddressbarURL.ToString();
				FString MaskedDomainName = "";
				bool bDomainNameFound = false;
				for (const auto& It : MaskedDomains)
				{
					for (const auto& ItDomainName : It.Key.CustomDomainNames)
					{
						if (ItDomainName.Equals(LastLoadedURL))
						{
							bDomainNameFound = true;
							MaskedDomainName = ItDomainName;
							break;
						}
					}

					if (bDomainNameFound)
					{
						break;
					}
				}

				AddressbarURL = FText::FromString(AddressbarUrlString.Replace((TEXT("%s"), *MaskedURL), (TEXT("%s"), *MaskedDomainName)));
			}

			Addressbar->SetText(AddressbarURL);
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
	static const std::regex RegPattern("^((http|https):\\/\\/)?(www.)?(?!.*(http|https|www.))[a-zA-Z0-9_-]+(\\.[a-zA-Z0-9]+)+((\\/)[\\w#]+)*(\\/\\w+\\?[a-zA-Z0-9_]+=\\w+(&[a-zA-Z0-9_]+=\\w+)*)?.+$");
	return std::regex_match(TCHAR_TO_UTF8(*InURL), RegPattern);
}

const bool UYetiOS_WebBrowser::Internal_IsBrowserURL(const FString& InURL) const
{
	return bSupportBrowserURLs && InURL.StartsWith(GetBrowserProtocolLink());
}

const bool UYetiOS_WebBrowser::Internal_FindMaskedURL(const FText& InURL, FString& OutMaskedURL) const
{
	if (MaskedDomains.Num() > 0)
	{
		const FString UrlText = InURL.ToLower().ToString();
		bool bCustomURLFound = false;
		for (const auto& It : MaskedDomains)
		{
			for (const auto& ItString : It.Key.CustomDomainNames)
			{
				if (UrlText.Contains(ItString, ESearchCase::IgnoreCase))
				{
					bCustomURLFound = true;
					OutMaskedURL = It.Value;
					return true;
				}
			}
		}
	}

	OutMaskedURL.Empty();
	return false;
}

#undef printlog
#undef printlog_vv
#undef printlog_error

#undef LOCTEXT_NAMESPACE
