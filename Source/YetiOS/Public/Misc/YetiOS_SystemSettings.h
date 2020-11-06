// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "YetiOS_Types.h"
#include "YetiOS_SystemSettings.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnThemeModeChanged, EThemeMode)
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
	EThemeMode CurrentTheme;

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
	const bool UpdateTheme(EThemeMode NewMode);

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
	EThemeMode GetCurrentTheme() const { return CurrentTheme; }

	UFUNCTION(BlueprintPure, BlueprintCosmetic, Category = "Yeti OS System Settings")
	bool GetShowProgramIcon() const { return bShowProgramIconOnWindows; }
};
