// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_HardDisk.generated.h"


UCLASS(Abstract)
class YETIOS_API UYetiOS_HardDisk : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

	friend class UYetiOS_BaseDevice;

private:

	/* Root directory of this HDD to boot the OS from. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Hard Disk")
	TSubclassOf<class UYetiOS_DirectoryRoot> RootDirectoryClass;

	/* Size of this HDD in MB (Megabyte). More size means you can have more files. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Hard Disk", meta = (UIMin = "40000", ClampMin = "40000", UIMax = "6000000", ClampMax = "8000000"))
	int32 HddCapacityInMB;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	int64 RemainingSpaceInBytes;

public:

	UYetiOS_HardDisk();

	static UYetiOS_HardDisk* CreateHDD(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	static int64 ConvertMegabyteToByte(const float& InMegabyteSize);

	bool ConsumeSpace(const float& SpaceInMB);

private:

	bool Internal_ConsumeSpace(const int64& InSpaceInBytes);
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
