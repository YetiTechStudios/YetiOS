// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_BaseHardware.h"

UYetiOS_BaseHardware::UYetiOS_BaseHardware()
{
	Name = Description = Model = Brand = FText::GetEmpty();
	Icon = nullptr;
	bHasWattage = true;
	Wattage = 0;

	InstalledDevice = nullptr;
}

EYetiOsHardwareInstallResult UYetiOS_BaseHardware::InstallToDevice(class UYetiOS_BaseDevice* ToDevice, const bool bRemoveIfInstalled /*= false*/)
{
	if (ToDevice == nullptr)
	{
		return EYetiOsHardwareInstallResult::HWINSTALL_NoDevice;
	}

	if (ToDevice == InstalledDevice)
	{
		return EYetiOsHardwareInstallResult::HWINSTALL_AlreadyInstalled;
	}

	InstalledDevice = ToDevice;
	return EYetiOsHardwareInstallResult::HWINSTALL_Success;
}

bool UYetiOS_BaseHardware::IsInstalled() const
{
	return InstalledDevice != nullptr;
}

class UYetiOS_BaseDevice* UYetiOS_BaseHardware::GetInstalledDevice() const
{
	return InstalledDevice;
}
