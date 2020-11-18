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

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<UYetiOS_DirectoryBase*> SystemDirectories;
	
public:

	UYetiOS_DirectoryRoot();

	void AddSystemDirectory(UYetiOS_DirectoryBase* InDirectoryToAdd);

	FORCEINLINE const TArray<UYetiOS_DirectoryBase*> GetSystemDirectories() const { return SystemDirectories; }
	
};
