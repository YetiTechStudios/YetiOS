// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_CPU.generated.h"

/*************************************************************************
* File Information:
YetiOS_CPU.h

* Description:
Represents a CPU object.
*************************************************************************/
UCLASS(Abstract)
class YETIOS_API UYetiOS_CPU : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()
	
private:

	/** Socket type. CPU and Motherboard must have the same socket types to be compatible. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS CPU")
	EYetiOsSocketType SocketType;

	/** Speed of this CPU in MegaHertz */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS CPU", meta = (UIMin = "1000", ClampMin = "1000", UIMax = "5000", ClampMax = "5500"))
	float SpeedInMHZ;

public:

	UYetiOS_CPU();

	/**
	* public static UYetiOS_CPU::CreateCPU
	* Creates a CPU object.
	* @param InMotherboard [UYetiOS_Motherboard*] Motherboard that owns this CPU.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any)
	* @return [UYetiOS_CPU*] New CPU that was created.
	**/
	static UYetiOS_CPU* CreateCPU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_CPU::GetCpuSpeed const
	* Gets the CPU speed in Mega Hertz.
	* @return [float] CPU Speed.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS CPU")	
	float GetCpuSpeed() const { return SpeedInMHZ; }

	FORCEINLINE const EYetiOsSocketType GetSocketType() const { return SocketType; }
};
