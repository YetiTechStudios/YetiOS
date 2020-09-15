// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "IYetiWebBrowserWindow.h"

class IYetiWebBrowserAdapter
{
public:

	virtual FString GetName() const = 0;

	virtual bool IsPermanent() const = 0;

	virtual void ConnectTo(const TSharedRef<IYetiWebBrowserWindow>& BrowserWindow) = 0;

	virtual void DisconnectFrom(const TSharedRef<IYetiWebBrowserWindow>& BrowserWindow) = 0;

};

class YETIWEBBROWSER_API FWebBrowserAdapterFactory 
{ 
public: 

	static TSharedRef<IYetiWebBrowserAdapter> Create(const FString& Name, UObject* JSBridge, bool IsPermanent); 

	static TSharedRef<IYetiWebBrowserAdapter> Create(const FString& Name, UObject* JSBridge, bool IsPermanent, const FString& ConnectScriptText, const FString& DisconnectScriptText);
}; 
