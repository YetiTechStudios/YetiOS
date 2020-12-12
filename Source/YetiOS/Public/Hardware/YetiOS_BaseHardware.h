// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "YetiOS_BaseHardware.generated.h"

/*************************************************************************
* File Information:
YetiOS_BaseHardware.h

* Description:
Base class for all hardwares
*************************************************************************/
UCLASS(Abstract, Blueprintable, DisplayName = "Hardware")
class YETIOS_API UYetiOS_BaseHardware : public UObject
{
	GENERATED_BODY()

protected:

	/** Name of this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	FText Name;

	/** Description for this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware", meta = (MultiLine = "true"))
	FText Description;

	/** Model name. This should be different to Name and should be unique. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	FText Model;

	/** Brand name. This can act as the company who made this. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	FText Brand;

	/** Price of this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	float Price;

	/** An icon for this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* Icon;

	/** List of devices this hardware is supported. If no class is specified, it is assumed all devices are supported. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	TArray<TSubclassOf<class UYetiOS_BaseDevice>> SupportedDeviceClasses;

	/** True if this hardware can be installed while the device is running */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	uint8 bCanInstallIfDeviceIsRunning : 1;

	/** If this hardware is removed, should it be considererd fatal for the owning device? */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	uint8 bFatalIfForceRemoved : 1;

	/** Does this hardware has wattage settings. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware")
	uint8 bHasWattage : 1;

	/** Power required by this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Hardware", meta = (UIMin = "0", ClampMin = "0", UIMax = "1000", EditCondition = "bHasWattage"))
	int32 Wattage;
	
	/** Points to the device which this hardware is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BaseDevice* InstalledDevice;

public:

	UYetiOS_BaseHardware();

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Hardware")
	/**
	* public UYetiOS_BaseHardware::InstallToDevice
	* Installs this hardware to given device.
	* @param ToDevice [class UYetiOS_BaseDevice*] Device to install to.
	* @param bRemoveIfInstalled [const bool] Should the hardware be forcefully removed? Might cause BSOD.
	* @return [EYetiOsHardwareInstallResult] Result of hardware removal.
	**/
	EYetiOsHardwareInstallResult InstallToDevice(class UYetiOS_BaseDevice* ToDevice, const bool bRemoveIfInstalled = false);

	/**
	* public UYetiOS_BaseHardware::RemoveFromDevice
	* Removes this hardware from its device.
	* @return [bool] True if the hardware was removed.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Hardware")	
	bool RemoveFromDevice();

	/**
	* public UYetiOS_BaseHardware::IsInstalled const
	* Checks if this hardware is installed or not.
	* @return [bool] True if this hardware is installed to a device.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Hardware")
	bool IsInstalled() const;

	/**
	* public UYetiOS_BaseHardware::GetInstalledDevice const
	* Returns the device this hardware is installed to. If not installed, returns null.
	* @return [class UYetiOS_BaseDevice*] Returns InstalledDevice or null if not installed.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Hardware")	
	class UYetiOS_BaseDevice* GetInstalledDevice() const;

	/**
	* public UYetiOS_BaseHardware::IsCompatibleWithDevice const
	* Checks if this hardware is compatible with given device.
	* @param InDevice [const class UYetiOS_BaseDevice*] Device to check against.
	* @return [const bool] True if this hardware is compatible..
	**/
	const bool IsCompatibleWithDevice(const class UYetiOS_BaseDevice* InDevice) const;

	/**
	* public UYetiOS_BaseHardware::GetName const
	* Returns the name of this device.
	* @return [FText] Device Name
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Hardware")	
	FText GetName() const { return Name; }

	/**
	* public UYetiOS_BaseHardware::GetModel const
	* Returns the model of this device.
	* @return [FText] Device Model
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Hardware")	
	FText GetModel() const { return Model; }

	/**
	* public UYetiOS_BaseHardware::GetBrand const
	* Returns the brand of this device.
	* @return [FText] Device Brand
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Hardware")
	FText GetBrand() const { return Brand; }

public:

	FORCEINLINE const bool IsFatalIfForceRemoved() const { return bFatalIfForceRemoved; }
};
