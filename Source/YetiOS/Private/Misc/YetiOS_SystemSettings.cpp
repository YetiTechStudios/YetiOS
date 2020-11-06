// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Misc/YetiOS_SystemSettings.h"
#include "Core/YetiOS_Core.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsSystemSettings, All, All)

#define printlog(Param1)					UE_LOG(LogYetiOsSystemSettings, Log, TEXT("%s"), *FString(Param1))
#define printlog_vv(Param1)					UE_LOG(LogYetiOsSystemSettings, VeryVerbose, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)				UE_LOG(LogYetiOsSystemSettings, Error, TEXT("%s"), *FString(Param1))

UYetiOS_SystemSettings::UYetiOS_SystemSettings()
{
	bShowProgramIconOnWindows = false;
	CurrentTheme = EThemeMode::Light;
}

void UYetiOS_SystemSettings::ToggleShowProgramIconOnWindows(const bool bShowIcon)
{
	bShowProgramIconOnWindows = bShowIcon;
	OnShowProgramIconChanged.Broadcast(bShowProgramIconOnWindows);
}

UYetiOS_SystemSettings* UYetiOS_SystemSettings::CreateSystemSettings(class UYetiOS_Core* InCore)
{
	if (InCore == nullptr)
	{
		printlog_error("Create System Settings failed. Reason: NULL OS.")
		return nullptr;
	}

	if (InCore->GetSystemSettingsClass() == NULL)
	{
		printlog_error(FString::Printf(TEXT("Create System Settings failed. Reason: No settings class provided for %s."), *InCore->GetOsName().ToString()));
		return nullptr;
	}

	const FString SystemSettingsName = FString::Printf(TEXT("SystemSettings_%s"), *InCore->GetOsName().ToString());
	UYetiOS_SystemSettings* ProxyObject = NewObject<UYetiOS_SystemSettings>(InCore, InCore->GetSystemSettingsClass(), *SystemSettingsName);
	printlog(FString::Printf(TEXT("System settings created: %s"), *SystemSettingsName));
	return ProxyObject;
}

const bool UYetiOS_SystemSettings::UpdateTheme(EThemeMode NewMode)
{
	if (CurrentTheme != NewMode)
	{
		printlog_vv(FString::Printf(TEXT("Current theme changed from %s to %s"), *ENUM_TO_STRING(EThemeMode, CurrentTheme), *ENUM_TO_STRING(EThemeMode, NewMode)));
		CurrentTheme = NewMode;
		OnThemeModeChanged.Broadcast(CurrentTheme);
		return true;
	}

	return false;
}

#undef printlog
#undef printlog_vv
#undef printlog_error
