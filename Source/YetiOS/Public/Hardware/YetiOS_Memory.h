// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Hardware/YetiOS_DeviceHardware.h"
#include "YetiOS_Memory.generated.h"

/*************************************************************************
* File Information:
YetiOS_Memory.h

* Description:
Represents a RAM Object.
*************************************************************************/
UCLASS(Abstract)
class YETIOS_API UYetiOS_Memory : public UYetiOS_DeviceHardware
{
	GENERATED_BODY()

private:

	/** Size of this memory */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS RAM")
	EYetiOsMemorySize Size;

public:

	UYetiOS_Memory();

	/**
	* public static UYetiOS_Memory::CreateRAM
	* Creates a RAM object.
	* @param InMotherboard [UYetiOS_Motherboard*] Motherboard that owns this RAM.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [UYetiOS_Memory*] New RAM that was created.
	**/
	static UYetiOS_Memory* CreateRAM(UYetiOS_Motherboard* InMotherboard, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_Memory::GetMemorySpeed const
	* Returns the speed of this memory.
	* @See GetMemorySizeImplementation
	* @return [float] Speed of this memory
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS CPU")	
	float GetMemorySpeed() const;
	
};
