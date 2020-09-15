// Copyright Epic Games, Inc. All Rights Reserved.

#include "YetiWebBrowserModule.h"
#include "YetiWebBrowserSingleton.h"
#include "Misc/App.h"
#include "Misc/EngineVersion.h"
#include "Interfaces/IPluginManager.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiBrowserModule, All, All)

#define LOG_NORMAL(StringParam)		UE_LOG(LogYetiBrowserModule, Display, TEXT("%s"), *FString(StringParam))
#define LOG_ERROR(StringParam)		UE_LOG(LogYetiBrowserModule, Error, TEXT("%s"), *FString(StringParam))

static FYetiWebBrowserSingleton* YetiWebBrowserSingleton = nullptr;

void FYetiWebBrowserModule::StartupModule()
{
	LOG_NORMAL("Starting Yeti Web Browser Module.");

	// Get the base directory of this plugin
	const FString BaseDir = IPluginManager::Get().FindPlugin("YetiOS")->GetBaseDir();

#if PLATFORM_WINDOWS
	static const FString PlatformName = "Win64";
#endif

	const FString ThirdPartyPath = FPaths::Combine(*BaseDir, TEXT("Binaries"), TEXT("ThirdParty"), TEXT("CEF"), *PlatformName);
	LOG_NORMAL(FString::Printf(TEXT("CEF binaries in %s"), *ThirdPartyPath));
	FPlatformProcess::PushDllDirectory(*ThirdPartyPath);
	CEFDLLHandle = Internal_LoadDependency(FPaths::Combine(*ThirdPartyPath, TEXT("libcef.dll")));
	ElfHandle = Internal_LoadDependency(FPaths::Combine(*ThirdPartyPath, TEXT("chrome_elf.dll")));		
	D3DHandle = Internal_LoadDependency(FPaths::Combine(*ThirdPartyPath, TEXT("d3dcompiler_47.dll")));
	GLESHandle = Internal_LoadDependency(FPaths::Combine(*ThirdPartyPath, TEXT("libGLESv2.dll")));
	EGLHandle = Internal_LoadDependency(FPaths::Combine(*ThirdPartyPath, TEXT("libEGL.dll")));
	FPlatformProcess::PopDllDirectory(*ThirdPartyPath);
}

void FYetiWebBrowserModule::ShutdownModule()
{
	Internal_FreeAllHandles();
	if (YetiWebBrowserSingleton != nullptr)
	{
		delete YetiWebBrowserSingleton;
		YetiWebBrowserSingleton = nullptr;
	}

	LOG_NORMAL("Shutting down Yeti Web Browser Module.");
}

bool FYetiWebBrowserModule::CustomInitialize(const FYetiWebBrowserInitSettings& WebBrowserInitSettings)
{
	if (YetiWebBrowserSingleton == nullptr)
	{
		YetiWebBrowserSingleton = new FYetiWebBrowserSingleton(WebBrowserInitSettings);
		return true;
	}
	return false;
}

void* FYetiWebBrowserModule::Internal_LoadDependency(const FString& Path)
{
	if (Path.IsEmpty())
	{
		return nullptr;
	}

	void* Handle = FPlatformProcess::GetDllHandle(*Path);
	if (!Handle)
	{
		int32 ErrorNum = FPlatformMisc::GetLastError();
		TCHAR ErrorMsg[1024];
		FPlatformMisc::GetSystemErrorMessage(ErrorMsg, 1024, ErrorNum);
		LOG_ERROR(FString::Printf(TEXT("Failed to get CEF DLL handle for %s: %s (%d)"), *Path, ErrorMsg, ErrorNum));
		return nullptr;
	}

	LOG_NORMAL(FString::Printf(TEXT("DLL handle loaded from %s"), *Path));
	return Handle;
}

void FYetiWebBrowserModule::Internal_FreeAllHandles()
{
	FPlatformProcess::FreeDllHandle(CEFDLLHandle);
	CEFDLLHandle = nullptr;
	FPlatformProcess::FreeDllHandle(ElfHandle);
	ElfHandle = nullptr;
	FPlatformProcess::FreeDllHandle(D3DHandle);
	D3DHandle = nullptr;
	FPlatformProcess::FreeDllHandle(GLESHandle);
	GLESHandle = nullptr;
	FPlatformProcess::FreeDllHandle(EGLHandle);
	EGLHandle = nullptr;
}

IYetiWebBrowserSingleton* FYetiWebBrowserModule::GetSingleton()
{
	if (YetiWebBrowserSingleton == nullptr)
	{
		YetiWebBrowserSingleton = new FYetiWebBrowserSingleton(FYetiWebBrowserInitSettings());
	}
	return YetiWebBrowserSingleton;
}

IMPLEMENT_MODULE(FYetiWebBrowserModule, YetiWebBrowser);

#undef LOG_NORMAL
#undef LOG_ERROR
