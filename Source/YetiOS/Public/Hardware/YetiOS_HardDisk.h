// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_HardDisk.generated.h"

/*************************************************************************
* File Information:
YetiOS_HardDisk.h

* Description:
Represents a Hard disk Object.
*************************************************************************/
UCLASS(Abstract)
class YETIOS_API UYetiOS_HardDisk : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

	friend class UYetiOS_Core;

private:

	/** Root directory of this HDD to boot the OS from. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Hard Disk")
	TSubclassOf<class UYetiOS_DirectoryRoot> RootDirectoryClass;

	/** Size of this HDD in MB (Megabyte). More size means you can have more files. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Hard Disk", meta = (UIMin = "40000", ClampMin = "40000", UIMax = "6000000", ClampMax = "8000000"))
	int32 HddCapacityInMB;

	/** Remaining space of this Hard disk. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	int64 RemainingSpaceInBytes;

public:

	UYetiOS_HardDisk();

	/**
	* public static UYetiOS_HardDisk::CreateHDD
	* Creates a HDD object.
	* @param InMotherboard [UYetiOS_Motherboard*] Motherboard that owns this HDD.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [UYetiOS_HardDisk*] New HDD that was created.
	**/
	static UYetiOS_HardDisk* CreateHDD(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	/**
	* public static UYetiOS_HardDisk::ConvertMegabyteToByte
	* Helper function to convert MB to bytes.
	* @param InMegabyteSize [const float&] Size in MB.
	* @return [int64] Converted MB to bytes result.
	**/
	static int64 ConvertMegabyteToByte(const float& InMegabyteSize);

	/**
	* public UYetiOS_HardDisk::ConsumeSpace
	* Tries to consume the given space from this HDD.
	* @param SpaceInMB [const float&] Space to consume.
	* @return [bool] True if there was enough space to reduce.
	**/
	bool ConsumeSpace(const float& SpaceInMB);

private:

	void Internal_UpdateRemainingSpace(const int64& InSize);

public:

	FORCEINLINE TSubclassOf<class UYetiOS_DirectoryRoot> GetRootDirectoryClass() const { return RootDirectoryClass; }
	FORCEINLINE const int64 GetRemainingSpace() const { return RemainingSpaceInBytes; }
	FORCEINLINE const float GetHddCapacity() const { return HddCapacityInMB; }
	FORCEINLINE const bool HasEnoughSpace(const float& TestSpaceInMB) const
	{
		return ConvertMegabyteToByte(TestSpaceInMB) <= RemainingSpaceInBytes;
	}
	
};
