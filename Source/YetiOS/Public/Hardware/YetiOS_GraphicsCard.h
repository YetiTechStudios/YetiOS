// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_GraphicsCard.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_GraphicsCard : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

public:

	static UYetiOS_GraphicsCard* CreateGPU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);
	
};
