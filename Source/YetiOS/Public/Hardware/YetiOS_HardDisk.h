// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_HardDisk.generated.h"


UCLASS(Abstract)
class YETIOS_API UYetiOS_HardDisk : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

private:

	/* Root directory of this HDD to boot the OS from. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Hard Disk")
	TSubclassOf<class UYetiOS_DirectoryRoot> RootDirectoryClass;

	/* Size of this HDD in GB (Gigabyte). More size means you can have more files. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Hard Disk", meta = (UIMin = "40", ClampMin = "40", UIMax = "6000", ClampMax = "8000"))
	float HddCapacityInGB;

public:

	UYetiOS_HardDisk();

	static UYetiOS_HardDisk* CreateHDD(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	FORCEINLINE TSubclassOf<class UYetiOS_DirectoryRoot> GetRootDirectoryClass() const { return RootDirectoryClass; }
	FORCEINLINE const float GetHddCapacity() const { return HddCapacityInGB; }
	
};
