// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_Memory.h"
#include "Hardware/YetiOS_Motherboard.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsRAM, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsRAM, Log, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsRAM, Error, TEXT("%s"), *FString(Param1))

UYetiOS_Memory::UYetiOS_Memory()
{
	Size = EYetiOsMemorySize::SIZE_256;
}

UYetiOS_Memory* UYetiOS_Memory::CreateRAM(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage)
{
	checkf(InMotherboard, TEXT("A motherboard is required to create RAM."));

	UYetiOS_Memory* ProxyRAM = NewObject<UYetiOS_Memory>(InMotherboard, InMotherboard->GetMotherboardDeviceClasses().Memory.Class);

	if (ProxyRAM->Size != InMotherboard->GetMinMemorySize())
	{
		printlog_error(FString::Printf(TEXT("Failed to install Memory '%s' to Motherboard '%s'. Incorrect size."), *ProxyRAM->GetName().ToString(), *InMotherboard->GetName().ToString()));
		ProxyRAM->ConditionalBeginDestroy();
		ProxyRAM = nullptr;
	}

	return ProxyRAM;
}

float UYetiOS_Memory::GetMemorySpeed() const
{
	return GetMemorySizeImplementation(Size);
}

#undef printlog
#undef printlog_error

