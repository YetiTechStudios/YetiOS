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

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FName ParentProgramIdentifier;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FString SaveSlotName;

	UPROPERTY(EditDefaultsOnly, Category = "Program Settings")
	int32 UserIndex;

	UPROPERTY()
	class UYetiOS_BaseProgram* OwningProgram;

public:

	UYetiOS_ProgramSettings();

	static UYetiOS_ProgramSettings* CreateSettings(const class UYetiOS_BaseProgram* InParentProgram, TSubclassOf<UYetiOS_ProgramSettings> InSettingsClass);

	template <class T>
	static T* CreateSettings(const class UYetiOS_BaseProgram* InParentProgram, TSubclassOf<UYetiOS_ProgramSettings> InSettingsClass)
	{
		static_assert(TIsDerivedFrom<T, UYetiOS_ProgramSettings>::IsDerived, "Create settings can only be used to create Program Settings instances. Please make sure you are casting to a class derived from UYetiOS_ProgramSettings.");
		return Cast<T>(CreateSettings(InParentProgram, InSettingsClass));
	}

	static UYetiOS_ProgramSettings* LoadSettings(const class UYetiOS_BaseProgram* InParentProgram);

	template <class T>
	static T* LoadSettings(const class UYetiOS_BaseProgram* InParentProgram)
	{
		static_assert(TIsDerivedFrom<T, UYetiOS_ProgramSettings>::IsDerived, "Load settings can only be used to create Program Settings instances. Please make sure you are casting to a class derived from UYetiOS_ProgramSettings.");
		return Cast<T>(LoadSettings(InParentProgram));
	}

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

	void Destroy();

private:

	void Internal_SetSaveSlotName();

protected:

	UFUNCTION(BlueprintPure, Category = "Yeti OS Program Settings")
	class UYetiOS_BaseProgram* GetOwningProgram() const;

	UFUNCTION(BlueprintNativeEvent, Category = "Yeti OS Program Settings", DisplayName = "Can Save")
	bool K2_CanSave() const;

	/**
	* protected UYetiOS_ProgramSettings::K2_PreSave
	* Event which is called before saving. Allows you to set data that needs to be saved.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Program Settings", DisplayName = "Pre Save")	
	void K2_PreSave();
};
