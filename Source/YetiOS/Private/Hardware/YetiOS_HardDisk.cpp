// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_HardDisk.h"
#include "Hardware/YetiOS_Motherboard.h"

UYetiOS_HardDisk::UYetiOS_HardDisk()
{
	HddCapacityInGB = 500.f;
}

UYetiOS_HardDisk* UYetiOS_HardDisk::CreateHDD(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage)
{
	checkf(InMotherboard, TEXT("A motherboard is required to create HDD."));

	UYetiOS_HardDisk* ProxyHDD = NewObject<UYetiOS_HardDisk>(InMotherboard, InMotherboard->GetMotherboardDeviceClasses().HardDiskClass);
	return ProxyHDD;
}
