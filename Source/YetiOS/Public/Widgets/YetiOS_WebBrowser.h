// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "YetiOS_WebBrowser.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebBrowserLoadStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebBrowserLoadCompleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWebBrowserLoadError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadBrowserURL);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAccessDenied);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUrlChanged, const FText&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBeforePopup, FString, URL, FString, Frame);

UENUM(BlueprintType)
enum class EBrowserURL : uint8
{
	About			UMETA(DisplayName = "about"),
	BrowserURLs		UMETA(DisplayName = "browser-urls"),
	Bookmarks		UMETA(DisplayName = "bookmarks"),
	History			UMETA(DisplayName = "history"),
	Quit			UMETA(DisplayName = "quit"),
	EMAX			UMETA(Hidden)	// Add new entries above this only!
};

USTRUCT(BlueprintType)
struct FWebHistory
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText URL;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDateTime DateAndTime;

	FORCEINLINE FWebHistory()
	{
		Title = FText::GetEmpty();
		URL = FText::GetEmpty();
		DateAndTime = FDateTime::Now();
	}

	FORCEINLINE FWebHistory(const FText& InTitle, const FString& InURL)
	{
		Title = InTitle;
		URL = FText::FromString(InURL);
		DateAndTime = FDateTime::Now();
	}
};

UCLASS()
class YETIOS_API UYetiOS_WebBrowser : public UWidget
{
	GENERATED_BODY()
	
private:
	/** URL that the browser will initially navigate to. The URL should include the protocol, eg http:// */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (AllowPrivateAccess = "true"))
	FString InitialURL;

	/** Should the browser window support transparency. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (AllowPrivateAccess = "true"))
	uint8 bSupportsTransparency : 1;

	/* If enabled, web browser widget will only load websites provided in WhitelistWebsites array. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (AllowPrivateAccess = "true"))
	uint8 bShowWhitelistOnly : 1;

	/* If enabled, web browser will add visited websites to History. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (AllowPrivateAccess = "true"))
	uint8 bEnableHistory : 1;

	/* If enabled, web browser will convert http to https. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (AllowPrivateAccess = "true"), AdvancedDisplay)
	uint8 bOnlyHTTPS : 1;

	/* If enabled, then support browser protocol link similar to chrome://. @See BrowserIdentifier */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (AllowPrivateAccess = "true"), AdvancedDisplay)
	uint8 bSupportBrowserURLs : 1;

	/* An internal identifier used for browser urls. @See UYetiOS_WebBrowser::GetBrowserProtocolLink() */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (EditCondition = "bSupportBrowserURLs", AllowPrivateAccess = "true"), AdvancedDisplay)
	FString BrowserIdentifier;

	/* If "Show Whitelist Only" is enabled then Web Browser will only allow to load web pages defined in this array. Accessing any web page not defined in this array will show error page. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Web Browser", meta = (EditCondition = "bShowWhitelistOnly", AllowPrivateAccess = "true"))
	TArray<FString> WhitelistWebsites;

	/* An array containing all web pages the user has visited. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	TArray<FWebHistory> History;

	UPROPERTY(BlueprintReadWrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UButton* BackButton;

	UPROPERTY(BlueprintReadWrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UButton* ForwardButton;

	UPROPERTY(BlueprintReadWrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UButton* ReloadButton;

	UPROPERTY(BlueprintReadWrite, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UEditableTextBox* Addressbar;

	TSharedPtr<class SWebBrowser> WebBrowserWidget;
	
public:

	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnAccessDenied OnAccessDenied;

	/* Called when a new tab needs to spawn. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnBeforePopup OnBeforePopup;

	/* Called when URL is changed. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnUrlChanged OnUrlChanged;

	/* Called when web browser starts loading a web page. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnWebBrowserLoadStarted OnLoadStarted;

	/* Called when web browser finishes loading a web page. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnWebBrowserLoadCompleted OnLoadCompleted;

	/* Called when web browser fails to load a web page. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnWebBrowserLoadError OnLoadError;

	/* Called when web browser tries to load internal browser URLs. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Web Browser|Events")
	FOnLoadBrowserURL OnLoadBrowserURL;

	UYetiOS_WebBrowser(const FObjectInitializer& ObjectInitializer);

	/**
	* public UYetiOS_WebBrowser::LoadURL
	* Loads the given URL with support for internal browser URLs.
	* Automatically adds https:// protocol if not present. So if you pass google.com to URL, it will be converted to https://google.com
	* @param URL [const FText &] URL to load. Adds https:// protocol if not present.
	* @return [const bool] True if URL is loaded.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Web Browser")
	const bool LoadURL(const FText& URL);

	/**
	* public UYetiOS_WebBrowser::LoadString
	* Load a string as data to create a web page.
	* @param Contents [FString] HTML string to load.
	* @param DummyURL [FString] Dummy URL for the page.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Web Browser")	
	void LoadString(FString Contents, FString DummyURL);
	
	/**
	* public UYetiOS_WebBrowser::ExecuteJavascript
	* Execute javascript on the current window
	* @param ScriptText [const FString&] Javascript to execute.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Web Browser")
	void ExecuteJavascript(const FString& ScriptText);

	/**
	* public UYetiOS_WebBrowser::GetTitleText const
	* Gets the title of the loaded URL.
	* @return [const FText] Title for the loaded URL.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Web Browser")
	const FText GetTitleText() const;

	/**
	* public UYetiOS_WebBrowser::GetUrl const
	* Returns the currently loaded URL.
	* @return [const FString] Currently loaded URL.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Web Browser")
	const FString GetUrl() const;

	/**
	* public UYetiOS_WebBrowser::GetAddressbarUrl const
	* Gets the URL that appears in the address bar, this may not be the URL that is currently loaded in the frame.
	* @return [FText] URL that appears in the address bar
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Web Browser")
	const FText GetAddressbarUrl() const;

	/**
	* public UYetiOS_WebBrowser::GetBrowserURLs const
	* Returns an array of valid browser urls.
	* @return [const TArray<FString>] Array string containing browser URLs.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Web Browser")	
	const TArray<FString> GetBrowserURLs() const;

	/**
	* public static UYetiOS_WebBrowser::GetBrowserUrlFromString
	* Returns the browser url type from the given string. Make sure you pass a valid name (not DisplayName) from EBrowserURL enum class.
	* @param InString [const FString&] EBrowserURL name. E.g: about, browserurls etc.
	* @param OutResult [EBrowserURL&] Returns the URL type if string is valid.
	* @return [const bool] True if the given string was converted successfully to EBrowserURL value.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Web Browser")
	static const bool GetBrowserUrlFromString(const FString& InString, EBrowserURL& OutResult);

	/**
	* public UYetiOS_WebBrowser::GetBrowserProtocolLink const
	* Gets the protocol link for this browser. Similar to chrome://
	* @See BrowserIdentifier variable.
	* @return [const FString] Returns browser protocol link.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Web Browser")
	const FString GetBrowserProtocolLink() const;

	// #YETI_OS TODO Not yet exposed to Blueprints. Slated for next update. :)
	const TArray<FWebHistory> GetHistory() const;

	/**
	* public UYetiOS_WebBrowser::AssignDelegates
	* Assign delegates to back, forward, reload buttons and address bar. Make sure you assign them first.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Web Browser")	
	void AssignDelegates();
		
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:
	
	UFUNCTION()
	void ReloadWebPage();

	void HandleOnUrlChanged(const FText& Text);
	void HandleOnLoadStart();
	void HandleOnLoadComplete();
	void HandleOnLoadError();

	bool HandleOnBeforePopup(FString URL, FString Frame);

private:

	UFUNCTION()
	void Internal_GoBack();

	UFUNCTION()
	void Internal_GoForward();

	const bool Internal_IsURLValid(const FString& InURL) const;
	const bool Internal_IsBrowserURL(const FString& InURL) const;
};
