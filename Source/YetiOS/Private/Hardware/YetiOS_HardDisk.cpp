// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_HardDisk.h"
#include "Hardware/YetiOS_Motherboard.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsHardDisk, All, All)

#define printlog_error(Param1)			UE_LOG(LogYetiOsHardDisk, Error, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsHardDisk, VeryVerbose, TEXT("%s"), *FString(Param1))

UYetiOS_HardDisk::UYetiOS_HardDisk()
{
	HddCapacityInMB = 40000.f;
}

UYetiOS_HardDisk* UYetiOS_HardDisk::CreateHDD(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage)
{
	checkf(InMotherboard, TEXT("A motherboard is required to create HDD."));

	UYetiOS_HardDisk* ProxyHDD = NewObject<UYetiOS_HardDisk>(InMotherboard, InMotherboard->GetMotherboardDeviceClasses().HardDiskClass);
	ProxyHDD->RemainingSpaceInBytes = ConvertMegabyteToByte(ProxyHDD->HddCapacityInMB);
	return ProxyHDD;
}

int64 UYetiOS_HardDisk::ConvertMegabyteToByte(const float& InMegabyteSize)
{
	return InMegabyteSize * 1000000;
}

bool UYetiOS_HardDisk::ConsumeSpace(const float& SpaceInMB)
{
	const int64& InSpaceInBytes = ConvertMegabyteToByte(SpaceInMB);;
	if (InSpaceInBytes <= RemainingSpaceInBytes)
	{
		RemainingSpaceInBytes -= InSpaceInBytes;
		const float ConvertedToGB = float(RemainingSpaceInBytes) / 1000000000.f;
		printlog_veryverbose(FString::Printf(TEXT("%s mb (%lld) consumed. Remaining %f GB (%lld)."), *FString::SanitizeFloat(SpaceInMB), InSpaceInBytes, ConvertedToGB, RemainingSpaceInBytes));
		return true;
	}

	printlog_error("Not enough space.");
	return false;
}

void UYetiOS_HardDisk::Internal_UpdateRemainingSpace(const int64& InSize)
{
	RemainingSpaceInBytes = InSize;
}

#undef printlog_error
#undef printlog_veryverbose
