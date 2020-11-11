// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_CPU.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_CPU : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()
	
private:

	/** Socket type. CPU and Motherboard must have the same socket types to be compatible. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS CPU")
	EYetiOsSocketType SocketType;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS CPU", meta = (UIMin = "1000", ClampMin = "1000", UIMax = "5000", ClampMax = "5500"))
	float SpeedInMHZ;

public:

	UYetiOS_CPU();

	static UYetiOS_CPU* CreateCPU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	UFUNCTION(BlueprintPure, Category = "Yeti OS CPU")
	float GetCpuSpeed() const { return SpeedInMHZ; }

	FORCEINLINE const EYetiOsSocketType GetSocketType() const { return SocketType; }
};
