// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_GraphicsCard.generated.h"

/*************************************************************************
* File Information:
YetiOS_GraphicsCard.h

* Description:
Represents a GPU Object.
*************************************************************************/
UCLASS(Abstract)
class YETIOS_API UYetiOS_GraphicsCard : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

public:

	/**
	* public static UYetiOS_GraphicsCard::CreateGPU
	* Creates a GPU object.
	* @param InMotherboard [UYetiOS_Motherboard*] Motherboard that owns this GPU.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any)
	* @return [UYetiOS_GraphicsCard*] New GPU that was created.
	**/
	static UYetiOS_GraphicsCard* CreateGPU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);
	
};
