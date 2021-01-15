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
	CurrentTheme = EYetiOsThemeMode::Light;

	ThemeColors[0].CollectionName = "Light";
	ThemeColors[0].SystemColors.Add(EYetiOsColorTypes::PrimaryColor, FLinearColor(0.693, 0.693, 0.693));
	ThemeColors[0].SystemColors.Add(EYetiOsColorTypes::SecondayColor, FLinearColor(0.9, 0.9, 0.9));
	ThemeColors[0].SystemColors.Add(EYetiOsColorTypes::AccentOne, FLinearColor(0.871, 0.871, 0.871));
	ThemeColors[0].SystemColors.Add(EYetiOsColorTypes::AccentTwo, FLinearColor(0.964, 0.964, 0.964));
	ThemeColors[0].SystemColors.Add(EYetiOsColorTypes::AccentThree, FLinearColor(0.05, 0.05, 0.05));

	ThemeColors[1].CollectionName = "Dark";
	ThemeColors[1].SystemColors.Add(EYetiOsColorTypes::PrimaryColor, FLinearColor(0.007, 0.007, 0.007));
	ThemeColors[1].SystemColors.Add(EYetiOsColorTypes::SecondayColor, FLinearColor(0.014, 0.014, 0.014));
	ThemeColors[1].SystemColors.Add(EYetiOsColorTypes::AccentOne, FLinearColor(0.03, 0.03, 0.03));
	ThemeColors[1].SystemColors.Add(EYetiOsColorTypes::AccentTwo, FLinearColor(0.018, 0.018, 0.018));
	ThemeColors[1].SystemColors.Add(EYetiOsColorTypes::AccentThree, FLinearColor(0.038, 0.038, 0.038));

	CurrentCollectionName = NAME_None;
}

bool UYetiOS_SystemSettings::UpdateTheme(EYetiOsThemeMode NewMode)
{
	if (CurrentTheme != NewMode)
	{
		printlog_vv(FString::Printf(TEXT("Current theme changed from %s to %s"), *ENUM_TO_STRING(EYetiOsThemeMode, CurrentTheme), *ENUM_TO_STRING(EYetiOsThemeMode, NewMode)));
		CurrentTheme = NewMode;
		OnThemeModeChanged.Broadcast(CurrentTheme);
		return true;
	}

	return false;
}

bool UYetiOS_SystemSettings::UpdateColorScheme(const FName& NewColorScheme)
{
	if (CurrentCollectionName.IsEqual(NewColorScheme, ENameCase::IgnoreCase) == false)
	{
		printlog_vv(FString::Printf(TEXT("Current color scheme changed from %s to %s"), *CurrentCollectionName.ToString(), *NewColorScheme.ToString()));
		CurrentCollectionName = NewColorScheme;
		OnColorSchemeChanged.Broadcast(CurrentCollectionName);
		return true;
	}

	return false;
}

void UYetiOS_SystemSettings::ToggleShowProgramIconOnWindows(const bool bShowIcon)
{
	bShowProgramIconOnWindows = bShowIcon;
	OnShowProgramIconChanged.Broadcast(bShowProgramIconOnWindows);
}

bool UYetiOS_SystemSettings::GetColorCollection(const FName InCollectionName, FYetiOsColorCollection& OutCollection) const
{
	if (InCollectionName.IsNone() == false)
	{
		for (const auto& It : ColorCollections)
		{
			if (It.CollectionName.IsEqual(InCollectionName, ENameCase::IgnoreCase))
			{
				OutCollection = It;
				return true;
			}
		}
	}

	printlog_error(FString::Printf(TEXT("Color Collection '%s' not found."), *InCollectionName.ToString()));
	OutCollection = FYetiOsColorCollection();
	return false;
}

TArray<FName> UYetiOS_SystemSettings::GetAllCollectionNames() const
{
	TArray<FName> ReturnResult;
	ReturnResult.Reserve(ColorCollections.Num());
	for (const auto& It : ColorCollections)
	{
		ReturnResult.Add(It.CollectionName);
	}

	return ReturnResult;
}

FLinearColor UYetiOS_SystemSettings::GetColorOfTypeFromCollection(const FName InCollectionName, EYetiOsColorTypes InColorType) const
{	
	FYetiOsColorCollection OutCollection;
	if (GetColorCollection(InCollectionName, OutCollection))
	{
		return OutCollection.SystemColors.FindRef(InColorType);
	}

	printlog_error(FString::Printf(TEXT("Failed to find color type '%s' from collection '%s'."), *InCollectionName.ToString(), *GetName()));
	return FLinearColor::Transparent;
}

FYetiOsColorCollection UYetiOS_SystemSettings::GetColorCollectionFromTheme(EYetiOsThemeMode InTheme) const
{
	return ThemeColors[(int32)InTheme];
}

FLinearColor UYetiOS_SystemSettings::GetColorFromCurrent(EYetiOsColorTypes InColorType) const
{
	return GetColorOfTypeFromCollection(CurrentCollectionName, InColorType);
}

FYetiOsColorCollection UYetiOS_SystemSettings::GetColorScheme(const FName& InCollectionName) const
{
	FYetiOsColorCollection OutCollection;
	GetColorCollection(InCollectionName, OutCollection);
	return OutCollection;
}

#undef printlog
#undef printlog_vv
#undef printlog_error
