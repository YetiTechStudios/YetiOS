// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "YetiOS_Types.h"
#include "YetiOS_SystemSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnThemeModeChanged, EYetiOsThemeMode)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShowProgramIconChanged, const bool)

UCLASS(Blueprintable, BlueprintType, hidedropdown, DisplayName = "System Settings")
class YETIOS_API UYetiOS_SystemSettings : public USaveGame
{
	GENERATED_BODY()

	friend class UYetiOS_Core;

private:

	UPROPERTY(EditDefaultsOnly, Category = "System Settings")
	uint8 bShowProgramIconOnWindows : 1;

	UPROPERTY(EditDefaultsOnly, Category = "System Settings")
	EYetiOsThemeMode CurrentTheme;

	UPROPERTY(EditDefaultsOnly, Category = "System Settings")
	TArray<FYetiOsColorCollection> ColorCollections;

public:

	FOnThemeModeChanged OnThemeModeChanged;
	FOnShowProgramIconChanged OnShowProgramIconChanged;

protected:

	static UYetiOS_SystemSettings* CreateSystemSettings(class UYetiOS_Core* InCore);

public:

	UYetiOS_SystemSettings();

	/**
	* public UYetiOS_SystemSettings::UpdateTheme
	* Updates the current theme mode to new one.
	* @param NewMode [EThemeMode] New theme mode to switch to.
	* @return [const bool] Returns true if theme was changed. False otherwise.
	**/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Yeti OS System Settings")
	const bool UpdateTheme(EYetiOsThemeMode NewMode);

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
	* public UYetiOS_SystemSettings::GetColorFromType const
	* Returns a linear color of the given type from given collection.
	* @param InCollectionName [const FName] Collection to search for.
	* @param InColorType [EYetiOsColorTypes] Type of color to return.
	* @return [FLinearColor] Linear color of given type.
	**/
	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")	
	FLinearColor GetColorFromType(const FName InCollectionName, EYetiOsColorTypes InColorType) const;
};
