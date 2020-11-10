// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "YetiOS_PortableDevice.generated.h"


UCLASS(Abstract, Blueprintable, DisplayName = "Portable Device")
class YETIOS_API UYetiOS_PortableDevice : public UYetiOS_BaseDevice
{
	GENERATED_BODY()
	
	FTimerHandle TimerHandle_ChargeBattery;
	FTimerHandle TimerHandle_ConsumeBattery;
	
private:

	/* Battery level for this portable device. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Portable Device", meta = (UIMin = "0", UIMax = "1", ClampMin = "0", ClampMax = "1"))
	float BatteryLevel;

	/* Details of currently installed battery. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Portable Device")
	FYetiOsPortableBattery InstalledBattery;

	/* Time taken to consume battery if not charging. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Portable Device", meta = (UIMin = "1", ClampMin = "0.2", UIMax = "300"))
	float BatteryConsumeTimerDelay;

	/* If true, user has already been notified of low battery. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bLowBatteryWarned : 1;

public:

	UYetiOS_PortableDevice();

	virtual const bool IsPortableDevice() const override final { return true; }


	virtual EYetiOsDeviceStartResult StartDevice(FYetiOsError& OutErrorMessage) override final;

	/**
	* public UYetiOS_PortableDevice::BeginBatteryCharge
	* Start charging timer.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Portable Device")	
	void BeginBatteryCharge();

	/**
	* public UYetiOS_PortableDevice::StopBatteryCharge
	* Stop charging timer.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Portable Device")
	void StopBatteryCharge();
		
	/**
	* public UYetiOS_PortableDevice::IsDeviceCharging const
	* Checks if the device is charging.
	* @return [const bool] True if the device is currently charging.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Portable Device")
	const bool IsDeviceCharging() const;

	/**
	* public UYetiOS_PortableDevice::GetBatteryLevel const
	* Gets the current battery level.
	* @return [const float] Current battery level
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Portable Device")
	const float GetBatteryLevel() const { return BatteryLevel; }

	/**
	* public UYetiOS_PortableDevice::GetBatteryHealth const
	* Gets the health of currently installed battery.
	* @param bNormalize [const bool] If true, return value will be in 0-1 range. If false, will return in 0-100 range.
	* @return [const float] Current health of installed battery.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Portable Device")
	const float GetBatteryHealth(const bool bNormalize = true) const { return InstalledBattery.GetBatteryHealth(bNormalize); }

private:

	void Internal_ConsumeBattery();
	void Internal_ChargeBattery();

protected:

	virtual void LoadSavedData(const class UYetiOS_SaveGame* InLoadGameInstance) override final;

protected:

	/**
	* protected UYetiOS_PortableDevice::K2_OnBatteryLevelChanged
	* Event that is called when battery level changes. Either by depleting or charging.
	* @param bConsumedBattery [const bool] True if this event was called when battery was consumed. if false, then battery is charging.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Portable Device", DisplayName = "OnBatteryLevelChanged")
	void K2_OnBatteryLevelChanged(const bool bConsumedBattery);

public:

	FORCEINLINE const float GetChargingSpeed() const { return (GetTimeToFullyRechargeInHours() * 3600.f) / 10.f; }
	FORCEINLINE const float GetTimeToFullyRechargeInHours() const { return InstalledBattery.GetTimeToFullyRechargeInHours(); }
};
