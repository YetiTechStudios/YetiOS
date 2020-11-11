// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_Motherboard.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Hardware/YetiOS_CPU.h"
#include "Hardware/YetiOS_Memory.h"
#include "Hardware/YetiOS_GraphicsCard.h"
#include "Hardware/YetiOS_HardDisk.h"
#include "Hardware/YetiOS_PowerSupply.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsMotherboard, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsMotherboard, Log, TEXT("%s"), *FString(Param1))
#define printlog_warn(Param1)			UE_LOG(LogYetiOsMotherboard, Warning, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsMotherboard, Error, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsMotherboard, VeryVerbose, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_Motherboard::UYetiOS_Motherboard()
{
	SocketType = EYetiOsSocketType::SOCKET_1150;
	MinimumMemorySize = EYetiOsMemorySize::SIZE_256;
}

UYetiOS_Motherboard* UYetiOS_Motherboard::CreateMotherboard(const UYetiOS_BaseDevice* InDevice, FYetiOsError& OutErrorMessage)
{
	OutErrorMessage = FYetiOsError();
	checkf(InDevice, TEXT("A device is required to create motherboard."));
	const FYetiOS_DeviceClasses& DeviceClasses = InDevice->GetDeviceClasses();

	if (DeviceClasses.MotherboardClass == NULL)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::MbErrorCode, YetiOS_CommonErrors::MbErrorException);
		printlog_error(OutErrorMessage.ErrorException.ToString());
		return nullptr;
	}

	if (DeviceClasses.CPU.Class == NULL || DeviceClasses.CPU.Total <= 0)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::CpuErrorCode, YetiOS_CommonErrors::CpuErrorException);
		printlog_error(OutErrorMessage.ErrorException.ToString());
		return nullptr;
	}

	if (DeviceClasses.Memory.Class == NULL || DeviceClasses.Memory.Total <= 0)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::RamErrorCode, YetiOS_CommonErrors::RamErrorException);
		printlog_error(OutErrorMessage.ErrorException.ToString());
		return nullptr;
	}

	if (DeviceClasses.GPU.Class == NULL || DeviceClasses.GPU.Total <= 0)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::GpuErrorCode, YetiOS_CommonErrors::GpuErrorException);
		printlog_error(OutErrorMessage.ErrorException.ToString());
		return nullptr;
	}

	if (DeviceClasses.HardDiskClass == NULL)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::HddErrorCode, YetiOS_CommonErrors::HddErrorException);
		printlog_error(OutErrorMessage.ErrorException.ToString());
		return nullptr;
	}

	if (DeviceClasses.PowerSupplyClass == NULL)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::PsuErrorCode, YetiOS_CommonErrors::PsuErrorException);
		printlog_error(OutErrorMessage.ErrorException.ToString());
		return nullptr;
	}

	UYetiOS_Motherboard* ProxyMotherboard = NewObject<UYetiOS_Motherboard>(const_cast<UYetiOS_BaseDevice*>(InDevice), DeviceClasses.MotherboardClass);
	ProxyMotherboard->MotherboardDeviceClasses = DeviceClasses;
	if (ProxyMotherboard->Internal_InstallHardwares(OutErrorMessage))
	{
		printlog("Motherboard created.");
		return ProxyMotherboard;
	}

	ProxyMotherboard->ConditionalBeginDestroy();
	ProxyMotherboard = nullptr;
	printlog_error("Failed to create Motherboard.");
	return nullptr;
}

const bool UYetiOS_Motherboard::Internal_InstallHardwares(FYetiOsError& OutErrorMessage)
{
	OutErrorMessage = FYetiOsError();

	UYetiOS_BaseDevice* OwningDevice = Cast<UYetiOS_BaseDevice>(GetOuter());
	if (IsCompatibleWithDevice(OwningDevice) == false)
	{
		OutErrorMessage = GetErrorStruct(YetiOS_CommonErrors::MbNotCompatibleErrorCode, YetiOS_CommonErrors::MbNotCompatibleErrorException);
		return false;
	}

	UYetiOS_CPU* MyCPU = UYetiOS_CPU::CreateCPU(this, OutErrorMessage);
	if (MyCPU == nullptr)
	{
		return false;
	}

	UYetiOS_Memory* MyRAM = UYetiOS_Memory::CreateRAM(this, OutErrorMessage);
	if (MyRAM == nullptr)
	{
		return false;
	}

	UYetiOS_GraphicsCard* MyGPU = UYetiOS_GraphicsCard::CreateGPU(this, OutErrorMessage);
	if (MyGPU == nullptr)
	{
		return false;
	}

	InstalledHDD = UYetiOS_HardDisk::CreateHDD(this, OutErrorMessage);
	if (InstalledHDD == nullptr)
	{
		return false;
	}

	InstalledPSU = UYetiOS_PowerSupply::CreatePSU(this, OutErrorMessage);
	if (InstalledPSU == nullptr)
	{
		return false;
	}

	InstalledCPU.Add(MyCPU, MotherboardDeviceClasses.CPU.Total);
	InstalledRAM.Add(MyRAM, MotherboardDeviceClasses.Memory.Total);
	InstalledGPU.Add(MyGPU, MotherboardDeviceClasses.GPU.Total);

	InstallToDevice(OwningDevice);
	MyCPU->InstallToDevice(OwningDevice);
	MyRAM->InstallToDevice(OwningDevice);
	MyGPU->InstallToDevice(OwningDevice);
	InstalledHDD->InstallToDevice(OwningDevice);
	InstalledPSU->InstallToDevice(OwningDevice);

	return true;
}

FText UYetiOS_Motherboard::GetSocketName() const
{
	return FText::FromString(GetSocketNameImplementation(SocketType));
}

class UYetiOS_CPU* UYetiOS_Motherboard::GetCpu(int32& OutTotal) const
{
	OutTotal = InstalledCPU.ItemCount;
	return InstalledCPU.Get<UYetiOS_CPU>();
}

class UYetiOS_Memory* UYetiOS_Motherboard::GetMemory(int32& OutTotal) const
{
	OutTotal = InstalledRAM.ItemCount;
	return InstalledRAM.Get<UYetiOS_Memory>();
}

class UYetiOS_GraphicsCard* UYetiOS_Motherboard::GetGraphicsCard(int32& OutTotal) const
{
	OutTotal = InstalledGPU.ItemCount;
	return InstalledGPU.Get<UYetiOS_GraphicsCard>();
}

const float UYetiOS_Motherboard::GetTotalCpuSpeed() const
{
	int32 Total = 0;
	UYetiOS_CPU* CPU = GetCpu(Total);
	return CPU->GetCpuSpeed() * Total;
}

const float UYetiOS_Motherboard::GetTotalMemorySize() const
{
	int32 Total = 0;
	UYetiOS_Memory* Memory = GetMemory(Total);
	return Memory->GetMemorySpeed() * Total;
}

#undef printlog
#undef printlog_warn
#undef printlog_error
#undef printlog_veryverbose

#undef LOCTEXT_NAMESPACE
