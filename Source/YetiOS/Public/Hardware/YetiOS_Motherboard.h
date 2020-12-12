// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_Motherboard.generated.h"

/** Represents a single hardware that is installed on this Motherboard */
USTRUCT()
struct FYetiOs_InstalledItem
{
	GENERATED_USTRUCT_BODY();
	
	/** Type of hardware */
	UPROPERTY()
	class UYetiOS_DeviceHardware* InItem;

	/** Total installed */
	int32 ItemCount;

	/**
	* public FYetiOs_InstalledItem::Add
	* Add to this struct.
	* @param InDevice [class UYetiOS_DeviceHardware*] Hardware that was created.
	* @param InTotal [const int32] Total number.
	**/
	FORCEINLINE void Add(class UYetiOS_DeviceHardware* InDevice, const int32 InTotal)
	{
		InItem = InDevice;
		ItemCount = InTotal;
	}

	/**
	* public FYetiOs_InstalledItem::Get const
	* Casts and returns the hardware type.
	* Example: InstalledCPU.Get<UYetiOS_CPU>()
	* @return [FORCEINLINE T*] Returns the casted version.
	**/
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

/*************************************************************************
* File Information:
YetiOS_Motherboard.h

* Description:
Represents a Motherboard object.
*************************************************************************/
UCLASS(Abstract)
class YETIOS_API UYetiOS_Motherboard : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()
	
private:

	/** CPU Socket type */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Motherboard")
	EYetiOsSocketType SocketType;

	/** Minimum memory size supported. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Motherboard")
	EYetiOsMemorySize MinimumMemorySize;
	
	/** Reference to the CPU that is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOs_InstalledItem InstalledCPU;

	/** Reference to the RAM that is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOs_InstalledItem InstalledRAM;

	/** Reference to the GPU that is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOs_InstalledItem InstalledGPU;

	/** Reference to the HDD that is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_HardDisk* InstalledHDD;

	/** Reference to the PSU that is installed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_PowerSupply* InstalledPSU;

	/** Holds reference to all the classes from Device. */
	FYetiOS_DeviceClasses MotherboardDeviceClasses;

public:

	UYetiOS_Motherboard();

	/**
	* public static UYetiOS_Motherboard::CreateMotherboard
	* Creates Mother board object.
	* @param InDevice [const UYetiOS_BaseDevice*] Device that owns this motherboard.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [UYetiOS_Motherboard*] Newly created motherboard.
	**/
	static UYetiOS_Motherboard* CreateMotherboard(const UYetiOS_BaseDevice* InDevice, FYetiOsError& OutErrorMessage);

private:

	/**
	* private UYetiOS_Motherboard::Internal_InstallHardwares
	* Installs all the default hardwares.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any)
	* @return [const bool] True if the hardware was installed successfully.
	**/
	const bool Internal_InstallHardwares(FYetiOsError& OutErrorMessage);

public:

	/**
	* public UYetiOS_Motherboard::GetSocketName const
	* Returns the name of the socket of this motherboard.
	* @return [FText] Socket name.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard")
	FText GetSocketName() const;
	
	/**
	* public UYetiOS_Motherboard::GetCpu const
	* Gets the Installed CPU.
	* @param OutTotal [int32&] Total cpu's that are installed.
	* @return [class UYetiOS_CPU*] CPU
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard")	
	class UYetiOS_CPU* GetCpu(int32& OutTotal) const;

	/**
	* public UYetiOS_Motherboard::GetMemory const
	* Gets the Installed RAM.
	* @param OutTotal [int32&] Total ram's that are installed.
	* @return [class UYetiOS_Memory*] RAM
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "ram"))	
	class UYetiOS_Memory* GetMemory(int32& OutTotal) const;

	/**
	* public UYetiOS_Motherboard::GetGraphicsCard const
	* Gets the Installed GPU.
	* @param OutTotal [int32&] Total gpu's that are installed.
	* @return [class UYetiOS_GraphicsCard*] GPU
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "gpu"))	
	class UYetiOS_GraphicsCard* GetGraphicsCard(int32& OutTotal) const;

	/**
	* public UYetiOS_Motherboard::GetHardDisk const
	* Gets the Installed HDD.
	* @return [class UYetiOS_HardDisk*] HDD
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "hdd, ssd"))	
	class UYetiOS_HardDisk* GetHardDisk() const { return InstalledHDD; }

	/**
	* public UYetiOS_Motherboard::GetPowerSupply const
	* Gets the Installed PSU.
	* @return [class UYetiOS_PowerSupply*] PSU
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard", meta = (Keywords = "psu"))	
	class UYetiOS_PowerSupply* GetPowerSupply() const { return InstalledPSU; }

	/**
	* public UYetiOS_Motherboard::GetSocketType const
	* Gets the Socket Type supported by this motherboard.
	* @return [EYetiOsSocketType] Socket Type.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Motherboard")	
	EYetiOsSocketType GetSocketType() const { return SocketType; }

public:

	/**
	* public UYetiOS_Motherboard::GetTotalCpuSpeed const
	* Gets total CPU speed. CpuSpeed * Total
	* @return [const float] Total Speed.
	**/
	const float GetTotalCpuSpeed() const;

	/**
	* public UYetiOS_Motherboard::GetTotalMemorySize const
	* Gets total memory size. MemorySize * Total
	* @return [const float] Total size
	**/
	const float GetTotalMemorySize() const;

	FORCEINLINE const EYetiOsMemorySize GetMinMemorySize() const { return MinimumMemorySize; }
	FORCEINLINE const FYetiOS_DeviceClasses& GetMotherboardDeviceClasses() const { return MotherboardDeviceClasses; }
};
