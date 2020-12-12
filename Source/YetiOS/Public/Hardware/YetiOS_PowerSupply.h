// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_PowerSupply.generated.h"

/*************************************************************************
* File Information:
YetiOS_PowerSupply.h

* Description:
Represents a PSU object.
*************************************************************************/
UCLASS(Abstract)
class YETIOS_API UYetiOS_PowerSupply : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

public:

	/**
	* public static UYetiOS_PowerSupply::CreatePSU
	* Creates PSU object.
	* @param InMotherboard [UYetiOS_Motherboard*] Motherboard that owns this PSU.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [UYetiOS_PowerSupply*] Newly created PSU.
	**/
	static UYetiOS_PowerSupply* CreatePSU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);
	
};
