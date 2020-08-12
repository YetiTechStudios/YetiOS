// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Hardware/YetiOS_BaseHardware.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Devices/YetiOS_DeviceManagerActor.h"

#define LOCTEXT_NAMESPACE "YetiOS"

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

	if (bRemoveIfInstalled && bFatalIfForceRemoved && InstalledDevice)
	{
		RemoveFromDevice();
		return EYetiOsHardwareInstallResult::HWINSTALL_DeviceRunning;
	}

	InstalledDevice = ToDevice;
	InstalledDevice->InstallHardware(this, false);
	return EYetiOsHardwareInstallResult::HWINSTALL_Success;
}

void UYetiOS_BaseHardware::RemoveFromDevice()
{
	InstalledDevice->RemoveHardware(this);
	InstalledDevice = nullptr;
	static const FText LOCAL_ERR_CODE = LOCTEXT("YetiOS_HW_ERROR_CODE", "ERR_HW_INSTALL_FAIL");
	static const FText LOCAL_EXCEPTION = LOCTEXT("YetiOS_HW_Exception", "FATAL ERROR: HARDWARE REMOVED.");
	static const FText LOCAL_EXCEPTION_DETAIL = LOCTEXT("YetiOS_HW_ExceptionDetail", "Hardware being used by the OS was removed.");
	AYetiOS_DeviceManagerActor::ShowBSOD(this, InstalledDevice, LOCAL_ERR_CODE, LOCAL_EXCEPTION, LOCAL_EXCEPTION_DETAIL);
}

bool UYetiOS_BaseHardware::IsInstalled() const
{
	return InstalledDevice != nullptr;
}

class UYetiOS_BaseDevice* UYetiOS_BaseHardware::GetInstalledDevice() const
{
	return InstalledDevice;
}

#undef LOCTEXT_NAMESPACE
