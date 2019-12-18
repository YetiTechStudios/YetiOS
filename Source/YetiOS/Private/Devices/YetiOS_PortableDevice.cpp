// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Devices/YetiOS_PortableDevice.h"
#include "Core/YetiOS_SaveGame.h"
#include "Core/YetiOS_Core.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsPortableDevice, All, All)

#define printlog_display(Param1)		UE_LOG(LogYetiOsPortableDevice, Display, TEXT("%s"), *FString(Param1))
#define printlog_warn(Param1)			UE_LOG(LogYetiOsPortableDevice, Warning, TEXT("%s"), *FString(Param1))

UYetiOS_PortableDevice::UYetiOS_PortableDevice()
{
	bLowBatteryWarned = false;
	BatteryLevel = 1.f;
	BatteryConsumeTimerDelay = 120.f;
}

EYetiOsDeviceStartResult UYetiOS_PortableDevice::StartDevice(FYetiOsError& OutErrorMessage)
{
	EYetiOsDeviceStartResult Local_StartResult = Super::StartDevice(OutErrorMessage);
	if (Local_StartResult == EYetiOsDeviceStartResult::DEVICESTART_Success || Local_StartResult == EYetiOsDeviceStartResult::DEVICESTART_PartialSuccess)
	{
		if (BatteryLevel <= InstalledBattery.LowBatteryWarningLevel)
		{
			GetOperatingSystem()->NotifyLowBattery(true);
		}

		GetOperatingSystem()->NotifyBatteryLevelChange(BatteryLevel);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ConsumeBattery, this, &UYetiOS_PortableDevice::Internal_ConsumeBattery, BatteryConsumeTimerDelay, true);
		Internal_ConsumeBattery();

		printlog_display(FString::Printf(TEXT("Current battery charge: %f%. Battery health: %f%"), BatteryLevel * 100.f, GetBatteryHealth(false)));
	}

	return Local_StartResult;
}

void UYetiOS_PortableDevice::BeginBatteryCharge()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ConsumeBattery);
	const float ChargingSpeed = GetChargingSpeed();
	printlog_display(FString::Printf(TEXT("Begin charging device %s. Charging every %f seconds. Will take %f hours to fully charge."), *GetDeviceName().ToString(), ChargingSpeed, GetTimeToFullyRechargeInHours()));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ChargeBattery, this, &UYetiOS_PortableDevice::Internal_ChargeBattery, ChargingSpeed, true);
	Internal_ChargeBattery();
	GetOperatingSystem()->NotifyLowBattery(false);
}

void UYetiOS_PortableDevice::StopBatteryCharge()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ChargeBattery);
	if (BatteryLevel <= InstalledBattery.LowBatteryWarningLevel)
	{
		GetOperatingSystem()->NotifyLowBattery(true);
	}

	printlog_display(FString::Printf(TEXT("Stopped battery charging for device %s. Current battery level: %f%."), *GetDeviceName().ToString(), BatteryLevel * 100.f));
	GetWorld()->GetTimerManager().SetTimer(TimerHandle_ConsumeBattery, this, &UYetiOS_PortableDevice::Internal_ConsumeBattery, BatteryConsumeTimerDelay, true);
}

const bool UYetiOS_PortableDevice::IsDeviceCharging() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(TimerHandle_ChargeBattery);
}

void UYetiOS_PortableDevice::Internal_ConsumeBattery()
{
	BatteryLevel -= 0.01;
	if (BatteryLevel > 0.f && BatteryLevel <= InstalledBattery.LowBatteryWarningLevel && bLowBatteryWarned == false)
	{
		bLowBatteryWarned = true;
		GetOperatingSystem()->NotifyLowBattery(true);
		printlog_warn(FString::Printf(TEXT("Battery level low for %s!"), *GetDeviceName().ToString()));
	}

	K2_OnBatteryLevelChanged(true);
	GetOperatingSystem()->NotifyBatteryLevelChange(BatteryLevel);
	printlog_display(FString::Printf(TEXT("%s consumed battery. Remaining %f%"), *GetDeviceName().ToString(), BatteryLevel * 100.f));
	if (BatteryLevel <= 0.f)
	{
		BatteryLevel = 0.f;
		ShutdownYetiDevice();
	}
}

void UYetiOS_PortableDevice::Internal_ChargeBattery()
{
	if (BatteryLevel < 1.f && BatteryLevel >= 0.f)
	{
		bLowBatteryWarned = false;
		BatteryLevel += 0.01;

		if (BatteryLevel >= 1.f)
		{
			printlog_display(FString::Printf(TEXT("Battery fully charged for device %s."), *GetDeviceName().ToString()));
			BatteryLevel = 1.f;
			StopBatteryCharge();
		}
		else
		{
			printlog_display(FString::Printf(TEXT("Battery charged for device %s. Current battery level: %f%."), *GetDeviceName().ToString(), BatteryLevel * 100.f));
		}

		K2_OnBatteryLevelChanged(false);
		GetOperatingSystem()->NotifyBatteryLevelChange(BatteryLevel);
	}
}

void UYetiOS_PortableDevice::LoadSavedData(const class UYetiOS_SaveGame* InLoadGameInstance)
{
	Super::LoadSavedData(InLoadGameInstance);
	if (InLoadGameInstance)
	{
		BatteryLevel = InLoadGameInstance->GetDeviceLoadData().SaveLoad_BatteryLevel;
	}	
}

const FYetiOsHardDisk UYetiOS_PortableDevice::GetHardDisk() const
{
	return PortableDeviceMotherBoard.HardDisk;
}

const float UYetiOS_PortableDevice::GetTotalCpuSpeed(const bool bWithDurability) const
{
	return PortableDeviceMotherBoard.GetTotalCpuSpeed(bWithDurability);
}

const float UYetiOS_PortableDevice::GetTotalMemorySize() const
{
	return PortableDeviceMotherBoard.GetTotalMemorySize();
}

const float UYetiOS_PortableDevice::GetMotherboardDurability() const
{
	return PortableDeviceMotherBoard.MotherboardDurability;
}

const bool UYetiOS_PortableDevice::MotherboardHasOnBoardGraphics() const
{
	return false;
}

const bool UYetiOS_PortableDevice::CpusAreOfCorrectType(FYetiOsCpu& OutIncorrectCpu) const
{
	return PortableDeviceMotherBoard.IsCpuOfCorrectType();
}

const bool UYetiOS_PortableDevice::IsGpuInstalled() const
{
	return true;
}

const bool UYetiOS_PortableDevice::HasEnoughPower() const
{
	return BatteryLevel > 0.f;
}

const FString UYetiOS_PortableDevice::GetSocketName() const
{
	return PortableDeviceMotherBoard.GetSocketName();
}

TSubclassOf<class UYetiOS_DirectoryRoot> UYetiOS_PortableDevice::GetRootDirectoryClass() const
{
	return PortableDeviceMotherBoard.HardDisk.RootDirectoryClass;
}

const TArray<FYetiOsCpu> UYetiOS_PortableDevice::GetAllCpus() const
{
	TArray<FYetiOsCpu> MyReturnArray;
	MyReturnArray.Add(PortableDeviceMotherBoard.MotherboardCpu);

	return MyReturnArray;
}

const TArray<FYetiOsMemory> UYetiOS_PortableDevice::GetAllMemory() const
{
	TArray<FYetiOsMemory> MyReturnArray;
	MyReturnArray.Add(PortableDeviceMotherBoard.MotherboardMemory);

	return MyReturnArray;
}

const TArray<FYetiOsGpu> UYetiOS_PortableDevice::GetAllGpu() const
{
	TArray<FYetiOsGpu> MyReturnArray;
	MyReturnArray.Add(PortableDeviceMotherBoard.MotherboardGpu);

	return MyReturnArray;
}

#undef printlog_display
#undef printlog_warn
