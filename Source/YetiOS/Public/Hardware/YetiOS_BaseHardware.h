// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "YetiOS_BaseHardware.generated.h"


UCLASS(Abstract, Blueprintable, DisplayName = "Hardware")
class YETIOS_API UYetiOS_BaseHardware : public UObject
{
	GENERATED_BODY()

protected:

	/* Name of this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	FText Name;

	/* Description for this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware", meta = (MultiLine = "true"))
	FText Description;

	/* Model name. This should be different to Name and should be unique. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	FText Model;

	/* Brand name. This can act as the company who made this. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	FText Brand;

	/** Price of this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	float Price;

	/* An icon for this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* Icon;

	/* List of devices this hardware is supported. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	TArray<TSubclassOf<class UYetiOS_BaseDevice>> SupportedDeviceClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	uint8 bCanInstallIfDeviceIsRunning : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	uint8 bFatalIfForceRemoved : 1;

	/* Does this hardware has wattage settings. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware")
	uint8 bHasWattage : 1;

	/* Power required by this hardware. */
	UPROPERTY(EditDefaultsOnly, Category = "Hardware", meta = (UIMin = "0", ClampMin = "0", UIMax = "1000", EditCondition = "bHasWattage"))
	int32 Wattage;
	
	/* Points to the device which this hardware is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BaseDevice* InstalledDevice;

public:

	UYetiOS_BaseHardware();

	UFUNCTION(BlueprintCallable, Category = "Hardware")
	EYetiOsHardwareInstallResult InstallToDevice(class UYetiOS_BaseDevice* ToDevice, const bool bRemoveIfInstalled = false);

	UFUNCTION(BlueprintCallable, Category = "Hardware")
	void RemoveFromDevice();

	/**
	* public UYetiOS_BaseHardware::IsInstalled const
	* Checks if this hardware is installed or not.
	* @return [bool] True if this hardware is installed to a device.
	**/
	UFUNCTION(BlueprintPure, Category = "Hardware")
	bool IsInstalled() const;

	/**
	* public UYetiOS_BaseHardware::GetInstalledDevice const
	* Returns the device this hardware is installed to. If not installed, returns null.
	* @return [class UYetiOS_BaseDevice*] Returns InstalledDevice or null if not installed.
	**/
	UFUNCTION(BlueprintPure, Category = "Hardware")	
	class UYetiOS_BaseDevice* GetInstalledDevice() const;

	const bool IsCompatibleWithDevice(const class UYetiOS_BaseDevice* InDevice) const;
public:

	FORCEINLINE const bool IsFatalIfForceRemoved() const { return bFatalIfForceRemoved; }
};
