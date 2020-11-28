// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "YetiOS_StationaryDevice.generated.h"

/**
 * 
 */
UCLASS(hidedropdown, Blueprintable, DisplayName = "Stationary Device")
class YETIOS_API UYetiOS_StationaryDevice : public UYetiOS_BaseDevice
{
	GENERATED_BODY()
	
public:

	UYetiOS_StationaryDevice();
};
