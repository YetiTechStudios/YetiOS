// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Devices/YetiOS_PortableDevice.h"
#include "Core/YetiOS_SaveGame.h"
#include "Core/YetiOS_Core.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsPortableDevice, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsPortableDevice, Log, TEXT("%s"), *FString(Param1))
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
	if (Local_StartResult == EYetiOsDeviceStartResult::DEVICESTART_Success)
	{
		if (BatteryLevel <= InstalledBattery.LowBatteryWarningLevel)
		{
			GetOperatingSystem()->NotifyLowBattery(true);
		}

		GetOperatingSystem()->NotifyBatteryLevelChange(BatteryLevel);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ConsumeBattery, this, &UYetiOS_PortableDevice::Internal_ConsumeBattery, BatteryConsumeTimerDelay, true);
		Internal_ConsumeBattery();

		printlog(FString::Printf(TEXT("Current battery charge: %f%s. Battery health: %f%s"), BatteryLevel * 100.f, *FString("%"), GetBatteryHealth(false), *FString("%")));
	}

	return Local_StartResult;
}

void UYetiOS_PortableDevice::BeginBatteryCharge()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ConsumeBattery);
	const float ChargingSpeed = GetChargingSpeed();
	printlog(FString::Printf(TEXT("Begin charging device %s. Charging every %f seconds. Will take %f hours to fully charge."), *GetDeviceName().ToString(), ChargingSpeed, GetTimeToFullyRechargeInHours()));
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

	printlog(FString::Printf(TEXT("Stopped battery charging for device %s. Current battery level: %f%s."), *GetDeviceName().ToString(), BatteryLevel * 100.f, *FString("%")));
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
	printlog(FString::Printf(TEXT("%s consumed battery. Remaining %f%s"), *GetDeviceName().ToString(), BatteryLevel * 100.f, *FString("%")));
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
			printlog(FString::Printf(TEXT("Battery fully charged for device %s."), *GetDeviceName().ToString()));
			BatteryLevel = 1.f;
			StopBatteryCharge();
		}
		else
		{
			printlog(FString::Printf(TEXT("Battery charged for device %s. Current battery level: %f%s."), *GetDeviceName().ToString(), BatteryLevel * 100.f, *FString("%")));
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

#undef printlog
#undef printlog_warn
