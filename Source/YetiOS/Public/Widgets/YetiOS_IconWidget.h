// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_Types.h"
#include "YetiOS_IconWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_IconWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()
	
	FTimerHandle TimerHandle_DoubleClick;

private:

	UPROPERTY(EditAnywhere, Category = "Yeti OS Icon Widget")
	float DoubleClickTime;

	uint8 NumberOfClicks;

public:

	UYetiOS_IconWidget(const FObjectInitializer& ObjectInitializer);

public:

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Icon Widget")
	void CheckForDoubleClick();

private:

	void Internal_OnDoubleClick();

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Icon Widget", DisplayName = "OnDoubleClick")
	void K2_OnDoubleClick();
};
