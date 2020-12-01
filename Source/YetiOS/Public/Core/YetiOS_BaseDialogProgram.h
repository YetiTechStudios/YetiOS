// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/YetiOS_BaseProgram.h"
#include "YetiOS_BaseDialogProgram.generated.h"

/**
 * 
 */
UCLASS(hidedropdown, Blueprintable, DisplayName = "Base Dialog Program")
class YETIOS_API UYetiOS_BaseDialogProgram : public UYetiOS_BaseProgram
{
	GENERATED_BODY()

	friend class UYetiOS_Core;

private:

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_DialogWidget* DialogWidget;
	
	void Internal_SetDialogWidget(class UYetiOS_DialogWidget* InDialogWidget);

public:

	virtual void CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown = false) override;

};
