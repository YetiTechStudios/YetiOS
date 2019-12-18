// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "YetiOS_StationaryDevice.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, DisplayName = "Stationary Device")
class YETIOS_API UYetiOS_StationaryDevice : public UYetiOS_BaseDevice
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Stationary Device")
	FYetiOsStationaryDeviceMotherBoard StationaryDeviceMotherBoard;
	
public:

	UYetiOS_StationaryDevice();

	/**
	* public UYetiOS_StationaryDevice::GetStationaryDeviceMotherBoard const
	* Gets device motherboard.
	* @return [FYetiOsStationaryDeviceMotherBoard] Returns MotherBoard.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Stationary Device")
	inline FYetiOsStationaryDeviceMotherBoard GetStationaryDeviceMotherBoard() const { return StationaryDeviceMotherBoard; }
	
protected:

	virtual const FYetiOsHardDisk GetHardDisk() const override final;
	virtual const float GetTotalCpuSpeed(const bool bWithDurability) const override final;
	virtual const float GetTotalMemorySize() const override final;
	virtual const float GetMotherboardDurability() const override final;
	virtual const bool MotherboardHasOnBoardGraphics() const override final;
	virtual const bool CpusAreOfCorrectType(FYetiOsCpu& OutIncorrectCpu) const override final;
	virtual const bool IsGpuInstalled() const override final;
	virtual const bool HasEnoughPower() const override final;
	virtual const FString GetSocketName() const override final;
	virtual TSubclassOf<class UYetiOS_DirectoryRoot> GetRootDirectoryClass() const override final;
	virtual const TArray<FYetiOsCpu> GetAllCpus() const override final;
	virtual const TArray<FYetiOsMemory> GetAllMemory() const override final;
	virtual const TArray<FYetiOsGpu> GetAllGpu() const override final;

public:

	FORCEINLINE const FYetiOsCpu GetCpu(const int32& AtIndex) const { return StationaryDeviceMotherBoard.MotherboardCpus[AtIndex]; }
};
