// Copyright Epic Games, Inc. All Rights Reserved.

#include "YetiWebBrowser.h"
#include "SYetiWebBrowser.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"

#if WITH_EDITOR
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialExpressionMaterialFunctionCall.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter2D.h"
#include "Materials/MaterialFunction.h"
#include "Factories/MaterialFactoryNew.h"
#include "AssetRegistryModule.h"
#include "PackageHelperFunctions.h"
#endif

#define LOCTEXT_NAMESPACE "WebBrowser"

/////////////////////////////////////////////////////
// UWebBrowser

UYetiWebBrowser::UYetiWebBrowser(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bIsVariable = true;
}

void UYetiWebBrowser::LoadURL(FString NewURL)
{
	if ( YetiWebBrowserWidget.IsValid() )
	{
		return YetiWebBrowserWidget->LoadURL(NewURL);
	}
}

void UYetiWebBrowser::LoadString(FString Contents, FString DummyURL)
{
	if ( YetiWebBrowserWidget.IsValid() )
	{
		return YetiWebBrowserWidget->LoadString(Contents, DummyURL);
	}
}

void UYetiWebBrowser::ExecuteJavascript(const FString& ScriptText)
{
	if (YetiWebBrowserWidget.IsValid())
	{
		return YetiWebBrowserWidget->ExecuteJavascript(ScriptText);
	}
}

FText UYetiWebBrowser::GetTitleText() const
{
	if ( YetiWebBrowserWidget.IsValid() )
	{
		return YetiWebBrowserWidget->GetTitleText();
	}

	return FText::GetEmpty();
}

FString UYetiWebBrowser::GetUrl() const
{
	if (YetiWebBrowserWidget.IsValid())
	{
		return YetiWebBrowserWidget->GetUrl();
	}

	return FString();
}

void UYetiWebBrowser::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);

	YetiWebBrowserWidget.Reset();
}

TSharedRef<SWidget> UYetiWebBrowser::RebuildWidget()
{
	if ( IsDesignTime() )
	{
		return SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("Yeti Web Browser", "Yeti Web Browser"))
			];
	}
	else
	{
		YetiWebBrowserWidget = SNew(SYetiWebBrowser)
			.InitialURL(InitialURL)
			.ShowControls(false)
			.SupportsTransparency(bSupportsTransparency)
			.OnUrlChanged(BIND_UOBJECT_DELEGATE(FOnTextChanged, HandleOnUrlChanged))
			.OnBeforePopup(BIND_UOBJECT_DELEGATE(FOnBeforePopupDelegate, HandleOnBeforePopup));

		return YetiWebBrowserWidget.ToSharedRef();
	}
}

void UYetiWebBrowser::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if ( YetiWebBrowserWidget.IsValid() )
	{

	}
}

void UYetiWebBrowser::HandleOnUrlChanged(const FText& InText)
{
	OnUrlChanged.Broadcast(InText);
}

bool UYetiWebBrowser::HandleOnBeforePopup(FString URL, FString Frame)
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
			TWeakObjectPtr<UYetiWebBrowser> WeakThis = this;
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

#if WITH_EDITOR

const FText UYetiWebBrowser::GetPaletteCategory()
{
	return LOCTEXT("Experimental", "Experimental");
}

#endif

/////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE
