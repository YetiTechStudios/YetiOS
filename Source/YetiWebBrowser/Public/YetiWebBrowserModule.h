// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"

class IYetiWebBrowserSingleton;

/**
 * WebBrowser initialization settings, can be used to override default init behaviors.
 */
struct YETIWEBBROWSER_API FYetiWebBrowserInitSettings
{
public:

	// The string which is appended to the browser's user-agent value.
	FString ProductVersion;

	/**
	 * Default constructor. Initializes all members with default behavior values.
	 */
	FYetiWebBrowserInitSettings()
	{
		ProductVersion = FString::Printf(TEXT("%s/%s UnrealEngine/%s Chrome/85.0.4183.83"), FApp::GetProjectName(), FApp::GetBuildVersion(), *FEngineVersion::Current().ToString());
	}
};

/**
 * WebBrowserModule interface
 */
class IYetiWebBrowserModule : public IModuleInterface
{
public:
	/**
	 * Get or load the Web Browser Module
	 * 
	 * @return The loaded module
	 */
	static inline IYetiWebBrowserModule& Get()
	{
		return FModuleManager::LoadModuleChecked< IYetiWebBrowserModule >("YetiWebBrowser");
	}
	
	/**
	 * Check whether the module has already been loaded
	 * 
	 * @return True if the module is loaded
	 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("YetiWebBrowser");
	}

	/**
	 * Customize initialization settings. You must call this before the first GetSingleton call, in order to override init settings.
	 * 
	 * @param WebBrowserInitSettings The custom settings.
	 * @return true if the settings were used to initialize the singleton. False if the call was ignored due to singleton already existing.
	 */
	virtual bool CustomInitialize(const FYetiWebBrowserInitSettings& WebBrowserInitSettings) = 0;

	/**
	 * Get the Web Browser Singleton
	 * 
	 * @return The Web Browser Singleton
	 */
	virtual IYetiWebBrowserSingleton* GetSingleton() = 0;

};

class FYetiWebBrowserModule : public IYetiWebBrowserModule
{
private:

	void* CEFDLLHandle = nullptr;
	void* ElfHandle = nullptr;
	void* D3DHandle = nullptr;
	void* GLESHandle = nullptr;
	void* EGLHandle = nullptr;

public:
	// IModuleInterface Interface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual IYetiWebBrowserSingleton* GetSingleton() override;
	virtual bool CustomInitialize(const FYetiWebBrowserInitSettings& WebBrowserInitSettings) override;

private:

	void* Internal_LoadDependency(const FString& Path);
	void Internal_FreeAllHandles();
};
