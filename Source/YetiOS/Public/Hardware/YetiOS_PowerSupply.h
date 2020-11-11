// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_PowerSupply.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_PowerSupply : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

public:

	static UYetiOS_PowerSupply* CreatePSU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);
	
};
