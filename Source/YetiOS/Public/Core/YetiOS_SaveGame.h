// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "YetiOS_Types.h"
#include "YetiOS_SaveGame.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class YETIOS_API UYetiOS_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
private:

	/** File name to save. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Save Game")
	FString SaveSlotName;

	/** For some platforms, master user index to identify the user doing the saving */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Save Game")
	uint32 UserIndex;

	/** Static save version. DO NOT CHANGE if not required. Newer save versions are NOT compatible with previous ones. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	float SaveVersion;

	/** Device data to save */
	UPROPERTY()
	FYetiOsDeviceSaveLoad DeviceData;

	/** Operating system data to save */
	UPROPERTY()
	FYetiOsOperatingSystemSaveLoad OsData;

	/** Directories to save */
	UPROPERTY()
	TArray<FYetiOsDirectorySaveLoad> DirectoryData;

	/** Programs to save */
	UPROPERTY()
	TArray<FYetiOsProgramSaveLoad> ProgramData;

public:

	UYetiOS_SaveGame();

	/**
	* public static UYetiOS_SaveGame::SaveGame
	* Saves the game from given device.
	* @param InDevice [const class UYetiOS_BaseDevice*] Device class to save game from.
	* @return [const bool] True if save game was successful.
	**/
	static const bool SaveGame(const class UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_SaveGame::LoadGame
	* Load game for given device.
	* @param InDevice [const class UYetiOS_BaseDevice*] Device to load.
	* @return [const UYetiOS_SaveGame*] Valid reference if load game was success.
	**/
	static const UYetiOS_SaveGame* LoadGame(const class UYetiOS_BaseDevice* InDevice);

	FORCEINLINE const FYetiOsDeviceSaveLoad GetDeviceLoadData() const { return DeviceData; }
	FORCEINLINE const FYetiOsOperatingSystemSaveLoad GetOsLoadData() const { return OsData; }
	FORCEINLINE const TArray<FYetiOsDirectorySaveLoad> GetDirectoriesData() const { return DirectoryData; }
	FORCEINLINE const TArray<FYetiOsProgramSaveLoad> GetProgramData() const { return ProgramData; }
};
