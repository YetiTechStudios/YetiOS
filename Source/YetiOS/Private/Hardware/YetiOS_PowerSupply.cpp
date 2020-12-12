// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_PowerSupply.h"
#include "Hardware/YetiOS_Motherboard.h"

UYetiOS_PowerSupply* UYetiOS_PowerSupply::CreatePSU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage)
{
	checkf(InMotherboard, TEXT("A motherboard is required to create RAM."));

	UYetiOS_PowerSupply* ProxyPSU = NewObject<UYetiOS_PowerSupply>(InMotherboard, InMotherboard->GetMotherboardDeviceClasses().PowerSupplyClass);
	return ProxyPSU;
}
