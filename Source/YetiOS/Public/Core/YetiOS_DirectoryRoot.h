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
UCLASS()
class YETIOS_API UYetiOS_DirectoryRoot : public UYetiOS_DirectoryBase
{
	GENERATED_BODY()
	
public:

	UYetiOS_DirectoryRoot();
	
};
