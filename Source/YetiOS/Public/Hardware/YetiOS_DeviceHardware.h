// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_BaseHardware.h"
#include "YetiOS_DeviceHardware.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_DeviceHardware : public UYetiOS_BaseHardware
{
	GENERATED_BODY()

protected:

	/** Temperature settings. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	FYetiOSTemperature Temperature;

	/** Current temperature of this hardware. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 CurrentTemperature;

public:

	UYetiOS_DeviceHardware();
	
};
