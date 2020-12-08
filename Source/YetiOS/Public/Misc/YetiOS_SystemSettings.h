// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YetiOS_Types.h"
#include "UObject/Object.h"
#include "YetiOS_SystemSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnThemeModeChanged, EYetiOsThemeMode)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnColorSchemeChanged, const FName&)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowProgramIconChanged, const bool)


/*************************************************************************
* File Information:
YetiOS_SystemSettings.h

* Description:
Class that defines some settings for the system.
*************************************************************************/
UCLASS(BlueprintType, hidedropdown, DisplayName = "System Settings")
class YETIOS_API UYetiOS_SystemSettings : public UObject
{
	GENERATED_BODY()

	friend class UYetiOS_Core;

private:

	/** If true show program icon on windows. Actual implementation must be done on UMG. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS System Settings")
	uint8 bShowProgramIconOnWindows : 1;

	/** Current theme of the system. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS System Settings")
	EYetiOsThemeMode CurrentTheme;

	/** Color scheme for themes. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS System Settings")
	FYetiOsColorCollection ThemeColors[(int32)EYetiOsThemeMode::MAX];

	/** Basic color scheme used for buttons and other stuff. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS System Settings")
	TArray<FYetiOsColorCollection> ColorCollections;

	/** Current name of the color scheme in use. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS System Settings", meta= (AllowPrivateAccess = "true"))
	FName CurrentCollectionName;

public:

	/** Delegate called when theme changes. */
	FOnThemeModeChanged OnThemeModeChanged;
	
	/** Delegate called when color scheme changes. */
	FOnColorSchemeChanged OnColorSchemeChanged;

	/** Delegate called when on show program icon changes. */
	FOnShowProgramIconChanged OnShowProgramIconChanged;

public:

	UYetiOS_SystemSettings();

	/**
	* public UYetiOS_SystemSettings::UpdateTheme
	* Updates the current theme mode to new one.
	* @param NewMode [EThemeMode] New theme mode to switch to.
	* @return [bool] Returns true if theme was changed. False otherwise.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS System Settings")
	bool UpdateTheme(EYetiOsThemeMode NewMode);

	/**
	* public UYetiOS_SystemSettings::UpdateColorScheme
	* Updates the color scheme to a new one.
	* @param NewColorScheme [const FName&] New color scheme to change to.
	* @return [bool] True if color scheme was changed successfully.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	bool UpdateColorScheme(const FName& NewColorScheme);

	/**
	* public UYetiOS_SystemSettings::ToggleShowProgramIconOnWindows
	* Changes so that the draggable window can show the program icon.
	* @param bShowIcon [const bool] True to show icon.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	void ToggleShowProgramIconOnWindows(const bool bShowIcon);
	
	/**
	* public UYetiOS_SystemSettings::GetCurrentTheme const
	* Returns the current theme.
	* @return [EThemeMode] Returns current theme.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	EYetiOsThemeMode GetCurrentTheme() const { return CurrentTheme; }

	/**
	* public UYetiOS_SystemSettings::GetShowProgramIcon const
	* Returns true if program icon is visible on draggable window.
	* @return [bool] True if icon is visible.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	bool GetShowProgramIcon() const { return bShowProgramIconOnWindows; }

	/**
	* public UYetiOS_SystemSettings::GetColorCollection const
	* Returns a specific color collection from list of collections.
	* @param InCollectionName [const FName] Collection name to search for.
	* @param OutCollection [FYetiOsColorCollection&] OUtputs the color collection struct.
	* @return [bool] True if a color collection with the given name exists.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	bool GetColorCollection(const FName InCollectionName, FYetiOsColorCollection& OutCollection) const;

	/**
	* public UYetiOS_SystemSettings::GetAllCollectionNames const
	* Returns an array of collection names from this settings.
	* @return [TArray<FName>] Array of collection names.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")
	TArray<FName> GetAllCollectionNames() const;

	/**
	* public UYetiOS_SystemSettings::GetColorOfTypeFromCollection const
	* Returns a linear color of the given type from given collection.
	* @param InCollectionName [const FName] Collection to search for.
	* @param InColorType [EYetiOsColorTypes] Type of color to return.
	* @return [FLinearColor] Linear color of given type.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	FLinearColor GetColorOfTypeFromCollection(const FName InCollectionName, EYetiOsColorTypes InColorType) const;

	/**
	* public UYetiOS_SystemSettings::GetColorCollectionFromCurrentTheme const
	* Returns the current color collection based on current theme.
	* @return [FYetiOsColorCollection] Color collection related to current theme.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	FYetiOsColorCollection GetColorCollectionFromTheme(EYetiOsThemeMode InTheme) const;

	/**
	* public UYetiOS_SystemSettings::GetColorFromCurrent
	* Returns color from current color collection that is set.
	* @param InColorType [EYetiOsColorTypes] Type of color to get.
	* @return [FLinearColor] Color of given type.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	FLinearColor GetColorFromCurrent(EYetiOsColorTypes InColorType) const;

	/**
	* public UYetiOS_SystemSettings::GetColorScheme const
	* Gets the color scheme of given collection.
	* @param InCollectionName [const FName&] Name of the collection.
	* @return [FYetiOsColorCollection] Returns the color scheme.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	FYetiOsColorCollection GetColorScheme(const FName& InCollectionName) const;

public:

	FORCEINLINE const FName GetCurrentCollectionName() const { return CurrentCollectionName; }
};
