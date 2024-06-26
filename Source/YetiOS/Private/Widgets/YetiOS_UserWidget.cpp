// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_UserWidget.h"
#include "Misc/YetiOS_SystemSettings.h"
#include "Core/YetiOS_Core.h"

void UYetiOS_UserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (OwningOS)
	{
		SetOperatingSystem(OwningOS);
	}
}

void UYetiOS_UserWidget::NativeDestruct()
{
	UnbindEvents();
	Super::NativeDestruct();
}

void UYetiOS_UserWidget::SetOperatingSystem(UYetiOS_Core* InOS)
{
	if (OwningOS != InOS)
	{
		OwningOS = InOS;
	}

	K2_SetOperatingSystem(OwningOS);
	UYetiOS_SystemSettings* OsSystemSettings = OwningOS->GetSystemSettings();
	if (OsSystemSettings)
	{
		K2_OnThemeChanged(OsSystemSettings->GetCurrentTheme());
		K2_OnColorSchemeChanged(OsSystemSettings->GetCurrentCollectionName());
		OnThemeChangedDelegateHandle = OsSystemSettings->OnThemeModeChanged.AddUFunction(this, FName("K2_OnThemeChanged"));
		OnColorSchemeChangedDelegateHandle = OsSystemSettings->OnColorSchemeChanged.AddUFunction(this, FName("K2_OnColorSchemeChanged"));
	}
}

void UYetiOS_UserWidget::UnbindEvents()
{
	if (OwningOS)
	{
		UYetiOS_SystemSettings* OsSystemSettings = OwningOS->GetSystemSettings();
		if (OsSystemSettings)
		{
			OsSystemSettings->OnThemeModeChanged.Remove(OnThemeChangedDelegateHandle);
			OsSystemSettings->OnColorSchemeChanged.Remove(OnColorSchemeChangedDelegateHandle);
			OnThemeChangedDelegateHandle.Reset();
			OnColorSchemeChangedDelegateHandle.Reset();
		}
	}
}
