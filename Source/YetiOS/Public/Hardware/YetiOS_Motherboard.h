// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_Motherboard.generated.h"

USTRUCT()
struct FYetiOs_InstalledItem
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY()
	class UYetiOS_DeviceHardware* InItem;

	int32 ItemCount;

	FORCEINLINE void Add(class UYetiOS_DeviceHardware* InDevice, const int32 InTotal)
	{
		InItem = InDevice;
		ItemCount = InTotal;
	}

	template <class T>
	FORCEINLINE T* Get() const
	{
		return Cast<T>(InItem);
	}

	FYetiOs_InstalledItem()
	{
		InItem = nullptr;
		ItemCount = INDEX_NONE;
	}
};

UCLASS(Abstract)
class YETIOS_API UYetiOS_Motherboard : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Motherboard")
	EYetiOsSocketType SocketType;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Motherboard")
	EYetiOsMemorySize MinimumMemorySize;
	
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOs_InstalledItem InstalledCPU;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOs_InstalledItem InstalledRAM;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOs_InstalledItem InstalledGPU;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_HardDisk* InstalledHDD;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_PowerSupply* InstalledPSU;

	FYetiOS_DeviceClasses MotherboardDeviceClasses;

public:

	UYetiOS_Motherboard();

	static UYetiOS_Motherboard* CreateMotherboard(const UYetiOS_BaseDevice* InDevice, FYetiOsError& OutErrorMessage);

private:

	const bool Internal_InstallHardwares(FYetiOsError& OutErrorMessage);

public:

	/**
	* public UYetiOS_Motherboard::GetSocketName const
	* Returns the name of the socket of this motherboard.
	* @return [FText] Socket name.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard")
	FText GetSocketName() const;
	
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard")
	class UYetiOS_CPU* GetCpu(int32& OutTotal) const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "ram"))
	class UYetiOS_Memory* GetMemory(int32& OutTotal) const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "gpu"))
	class UYetiOS_GraphicsCard* GetGraphicsCard(int32& OutTotal) const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "hdd, ssd"))
	class UYetiOS_HardDisk* GetHardDisk() const { return InstalledHDD; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "psu"))
	class UYetiOS_PowerSupply* GetPowerSupply() const { return InstalledPSU; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard")
	EYetiOsSocketType GetSocketType() const { return SocketType; }

public:

	const float GetTotalCpuSpeed() const;
	const float GetTotalMemorySize() const;

	FORCEINLINE const EYetiOsMemorySize GetMinMemorySize() const { return MinimumMemorySize; }
	FORCEINLINE const FYetiOS_DeviceClasses& GetMotherboardDeviceClasses() const { return MotherboardDeviceClasses; }
};
