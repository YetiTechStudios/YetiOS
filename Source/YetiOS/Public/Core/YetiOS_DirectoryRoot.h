// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "YetiOS_DirectoryRoot.generated.h"

/*************************************************************************
* File Information:
YetiOS_DirectoryRoot.h

* Description:
This is the main root directory for the OS (if OS requires root directory).
Note that this is inherited from the base class because we don't want any
properties of a normal directory.
*************************************************************************/
UCLASS(DisplayName = "Root Directory")
class YETIOS_API UYetiOS_DirectoryRoot : public UYetiOS_DirectoryBase
{
	GENERATED_BODY()

private:

	/** Array of system directories. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<UYetiOS_DirectoryBase*> SystemDirectories;
	
public:

	UYetiOS_DirectoryRoot();

	/** Add given directory as system. 
	* Will trigger an ensure assert if directory cannot be added. 
	*/
	void AddSystemDirectory(UYetiOS_DirectoryBase* InDirectoryToAdd);

	/**
	* public UYetiOS_DirectoryRoot::GetSystemDirectories const
	* Returns an array of system directories.
	* @return [FORCEINLINE const TArray<UYetiOS_DirectoryBase*>] 
	**/
	FORCEINLINE const TArray<UYetiOS_DirectoryBase*> GetSystemDirectories() const { return SystemDirectories; }
	
};
