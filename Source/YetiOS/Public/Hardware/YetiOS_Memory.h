// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_Memory.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_Memory : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS RAM")
	EYetiOsMemorySize Size;

public:

	UYetiOS_Memory();

	static UYetiOS_Memory* CreateRAM(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	UFUNCTION(BlueprintPure, Category = "Yeti OS CPU")
	float GetMemorySpeed() const;
	
};
