// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_GraphicsCard.h"
#include "Hardware/YetiOS_Motherboard.h"

UYetiOS_GraphicsCard* UYetiOS_GraphicsCard::CreateGPU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage)
{
	checkf(InMotherboard, TEXT("A motherboard is required to create GPU."));

	UYetiOS_GraphicsCard* ProxyGPU = NewObject<UYetiOS_GraphicsCard>(InMotherboard, InMotherboard->GetMotherboardDeviceClasses().GPU.Class);
	return ProxyGPU;
}
