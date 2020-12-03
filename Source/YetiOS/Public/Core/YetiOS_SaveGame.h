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

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Save Game")
	FString SaveSlotName;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Save Game")
	uint32 UserIndex;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	float SaveVersion;

	UPROPERTY()
	FYetiOsDeviceSaveLoad DeviceData;

	UPROPERTY()
	FYetiOsOperatingSystemSaveLoad OsData;

	UPROPERTY()
	TArray<FYetiOsDirectorySaveLoad> DirectoryData;

	UPROPERTY()
	TArray<FYetiOsProgramSaveLoad> ProgramData;

public:

	UYetiOS_SaveGame();

	static const bool SaveGame(const class UYetiOS_BaseDevice* InDevice);
	static const UYetiOS_SaveGame* LoadGame(const class UYetiOS_BaseDevice* InDevice);

	FORCEINLINE const FYetiOsDeviceSaveLoad GetDeviceLoadData() const { return DeviceData; }
	FORCEINLINE const FYetiOsOperatingSystemSaveLoad GetOsLoadData() const { return OsData; }
	FORCEINLINE const TArray<FYetiOsDirectorySaveLoad> GetDirectoriesData() const { return DirectoryData; }
	FORCEINLINE const TArray<FYetiOsProgramSaveLoad> GetProgramData() const { return ProgramData; }
};
