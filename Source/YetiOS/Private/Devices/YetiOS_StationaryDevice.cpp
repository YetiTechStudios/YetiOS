// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Devices/YetiOS_StationaryDevice.h"
#include "Core/YetiOS_DirectoryRoot.h"

UYetiOS_StationaryDevice::UYetiOS_StationaryDevice()
{
	StationaryDeviceMotherBoard = FYetiOsStationaryDeviceMotherBoard();
	StationaryDeviceMotherBoard.HardDisk.RootDirectoryClass = UYetiOS_DirectoryRoot::StaticClass();
}

const FYetiOsHardDisk UYetiOS_StationaryDevice::GetHardDisk() const
{
	return StationaryDeviceMotherBoard.HardDisk;
}

const float UYetiOS_StationaryDevice::GetTotalCpuSpeed(const bool bWithDurability) const
{
	return StationaryDeviceMotherBoard.GetTotalCpuSpeed(bWithDurability);
}

const float UYetiOS_StationaryDevice::GetTotalMemorySize() const
{
	return StationaryDeviceMotherBoard.GetTotalMemorySize();
}

const float UYetiOS_StationaryDevice::GetMotherboardDurability() const
{
	return StationaryDeviceMotherBoard.MotherboardDurability;
}

const bool UYetiOS_StationaryDevice::MotherboardHasOnBoardGraphics() const
{
	return StationaryDeviceMotherBoard.bHasOnboardGraphics;
}

const bool UYetiOS_StationaryDevice::CpusAreOfCorrectType(FYetiOsCpu& OutIncorrectCpu) const
{
	return StationaryDeviceMotherBoard.AllCpusAreOfCorrectSocketType(OutIncorrectCpu);
}

const bool UYetiOS_StationaryDevice::IsGpuInstalled() const
{
	return StationaryDeviceMotherBoard.HasGPUInstalled();
}

const bool UYetiOS_StationaryDevice::HasEnoughPower() const
{
	return StationaryDeviceMotherBoard.HasEnoughPower();
}

const FString UYetiOS_StationaryDevice::GetSocketName() const
{
	return StationaryDeviceMotherBoard.GetSocketName();
}

TSubclassOf<class UYetiOS_DirectoryRoot> UYetiOS_StationaryDevice::GetRootDirectoryClass() const
{
	return StationaryDeviceMotherBoard.HardDisk.RootDirectoryClass;
}

const TArray<FYetiOsCpu> UYetiOS_StationaryDevice::GetAllCpus() const
{
	return StationaryDeviceMotherBoard.MotherboardCpus;
}

const TArray<FYetiOsMemory> UYetiOS_StationaryDevice::GetAllMemory() const
{
	return StationaryDeviceMotherBoard.MotherboardMemories;
}

const TArray<FYetiOsGpu> UYetiOS_StationaryDevice::GetAllGpu() const
{
	return StationaryDeviceMotherBoard.MotherboardGpus;
}
