// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_CPU.h"
#include "Hardware/YetiOS_Motherboard.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsCPU, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsCPU, Log, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsCPU, Error, TEXT("%s"), *FString(Param1))

UYetiOS_CPU::UYetiOS_CPU()
{
	SocketType = EYetiOsSocketType::SOCKET_1150;
	SpeedInMHZ = 1000.f;
}

UYetiOS_CPU* UYetiOS_CPU::CreateCPU(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage)
{
	checkf(InMotherboard, TEXT("A motherboard is required to create CPU."));

	UYetiOS_CPU* ProxyCPU = NewObject<UYetiOS_CPU>(InMotherboard, InMotherboard->GetMotherboardDeviceClasses().CPU.Class);
	if (ProxyCPU->SocketType != InMotherboard->GetSocketType())
	{
		printlog_error(FString::Printf(TEXT("Failed to install CPU '%s' to Motherboard '%s'. Incorrect socket type."), *ProxyCPU->GetName().ToString(), *InMotherboard->GetName().ToString()));
		ProxyCPU->ConditionalBeginDestroy();
		ProxyCPU = nullptr;
	}

	return ProxyCPU;
}

#undef printlog
#undef printlog_error
