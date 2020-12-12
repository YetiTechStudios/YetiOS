// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "YetiOS_ProgramSettings.generated.h"

/*************************************************************************
* File Information:
YetiOS_ProgramSettings.h

* Description:
Base class for program settings.
*************************************************************************/
UCLASS(Blueprintable, hidedropdown, DisplayName = "Program Settings")
class YETIOS_API UYetiOS_ProgramSettings : public USaveGame
{
	GENERATED_BODY()
	
private:

	/** Identifier of the owning program */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FName ParentProgramIdentifier;

	/** Auto generated save slot name. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FString SaveSlotName;

	/** For some platforms, master user index to identify the user doing the saving */
	UPROPERTY(EditDefaultsOnly, Category = "Program Settings")
	int32 UserIndex;

	/** Program that owns this settings. */
	UPROPERTY()
	class UYetiOS_BaseProgram* OwningProgram;

public:

	UYetiOS_ProgramSettings();

	/**
	* public static UYetiOS_ProgramSettings::CreateSettings
	* Creates the settings object for the given program.
	* @param InParentProgram [const class UYetiOS_BaseProgram*] Program to create settings for.
	* @param InSettingsClass [TSubclassOf<UYetiOS_ProgramSettings>] Settings class to create from.
	* @return [UYetiOS_ProgramSettings*] Newly created settings object.
	**/
	static UYetiOS_ProgramSettings* CreateSettings(const class UYetiOS_BaseProgram* InParentProgram, TSubclassOf<UYetiOS_ProgramSettings> InSettingsClass);

	/**
	* public static UYetiOS_ProgramSettings::CreateSettings
	* Helper function that casts to the given settings type.
	* @See CreateSettings
	**/
	template <class T>
	static T* CreateSettings(const class UYetiOS_BaseProgram* InParentProgram, TSubclassOf<UYetiOS_ProgramSettings> InSettingsClass)
	{
		static_assert(TIsDerivedFrom<T, UYetiOS_ProgramSettings>::IsDerived, "Create settings can only be used to create Program Settings instances. Please make sure you are casting to a class derived from UYetiOS_ProgramSettings.");
		return Cast<T>(CreateSettings(InParentProgram, InSettingsClass));
	}

	/**
	* public static UYetiOS_ProgramSettings::LoadSettings
	* Load settings for given program.
	* @param InParentProgram [const class UYetiOS_BaseProgram*] Program to load settings for.
	* @return [UYetiOS_ProgramSettings*] Newly created program settings.
	**/
	static UYetiOS_ProgramSettings* LoadSettings(const class UYetiOS_BaseProgram* InParentProgram);

	/**
	* public static UYetiOS_ProgramSettings::LoadSettings
	* Helper function that casts to the given settings type.
	* @See LoadSettings
	**/
	template <class T>
	static T* LoadSettings(const class UYetiOS_BaseProgram* InParentProgram)
	{
		static_assert(TIsDerivedFrom<T, UYetiOS_ProgramSettings>::IsDerived, "Load settings can only be used to create Program Settings instances. Please make sure you are casting to a class derived from UYetiOS_ProgramSettings.");
		return Cast<T>(LoadSettings(InParentProgram));
	}

	/**
	* public static UYetiOS_ProgramSettings::SaveProgramSettings
	* Save program settings for given program
	* @param InProgram [class UYetiOS_BaseProgram*] Program to save settings for.
	* @return [bool] True if settings were saved.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Global")	
	static bool SaveProgramSettings(class UYetiOS_BaseProgram* InProgram);

private:
	/**
	* private UYetiOS_ProgramSettings::Internal_SaveSettings
	* DO NOT CALL THIS DIRECTLY. Use SaveProgramSettings instead.
	* Save settings to file. Make sure to implement Pre Save event. Calling this function will first call Can Save (K2_CanSave in C++) to make sure settings can be saved.
	* @See Can Save - Native implementation checks if the owning program is allowed to save and for most cases it should be fine but you can override Can Save event to have any custom implementation.
	* @See Pre Save - An event that you have to implement in Blueprint to collect whatever data you want to save.
	* @return [bool] Returns if UGameplayStatics::SaveGameToSlot call was success or not.
	**/
	bool Internal_SaveSettings();

public:

	/**
	* public UYetiOS_ProgramSettings::Destroy
	* Destroys this object.
	**/
	void Destroy();

private:

	/**
	* private UYetiOS_ProgramSettings::Internal_SetSaveSlotName
	* Sets the SaveSlotName variable.
	**/
	void Internal_SetSaveSlotName();

protected:

	/**
	* protected UYetiOS_ProgramSettings::GetOwningProgram const
	* Returns the program that owns this settings.
	* @return [class UYetiOS_BaseProgram*] OwningProgram
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Program Settings")
	class UYetiOS_BaseProgram* GetOwningProgram() const;

	/**
	* protected UYetiOS_ProgramSettings::K2_CanSave const
	* Function that determines if data can be saved. This can be overridden in Blueprints and native implementation simply checks if Device Manager can save game.
	* @return [bool] True if saving can be done.
	**/
	UFUNCTION(BlueprintNativeEvent, Category = "Yeti OS Program Settings", DisplayName = "Can Save")	
	bool K2_CanSave() const;

	/**
	* protected UYetiOS_ProgramSettings::K2_PreSave
	* Event which is called before saving. Allows you to set data that needs to be saved.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Program Settings", DisplayName = "Pre Save")	
	void K2_PreSave();

private:

	/**
	* private UYetiOS_ProgramSettings::Internal_GetSaveSlotName const
	* Auto generates the save slot name in format ProgramIdentifier_OwningDeviceName.
	* @return [const FString] 
	**/
	inline const FString Internal_GetSaveSlotName() const;
};
