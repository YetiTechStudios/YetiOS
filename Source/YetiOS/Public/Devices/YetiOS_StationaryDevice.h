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

public:

	FORCEINLINE const FYetiOsCpu GetCpu(const int32& AtIndex) const { return StationaryDeviceMotherBoard.MotherboardCpus[AtIndex]; }
};
