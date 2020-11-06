// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_UserWidget.h"
#include "Misc/YetiOS_SystemSettings.h"
#include "Core/YetiOS_Core.h"

void UYetiOS_UserWidget::NativeDestruct()
{
	UYetiOS_SystemSettings* OsSystemSettings = OwningOS->GetSystemSettings();
	if (OsSystemSettings)
	{
		OsSystemSettings->OnThemeModeChanged.Remove(OnThemeChangedDelegateHandle);
		OnThemeChangedDelegateHandle.Reset();
	}
	Super::NativeDestruct();
}

void UYetiOS_UserWidget::SetOperatingSystem(UYetiOS_Core* InOS)
{
	OwningOS = InOS;
	K2_SetOperatingSystem(OwningOS);
	UYetiOS_SystemSettings* OsSystemSettings = OwningOS->GetSystemSettings();
	if (OsSystemSettings)
	{
		K2_OnThemeChanged(OsSystemSettings->GetCurrentTheme());
		OnThemeChangedDelegateHandle = OsSystemSettings->OnThemeModeChanged.AddUFunction(this, FName("K2_OnThemeChanged"));
	}
}
