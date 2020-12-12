// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_Types.h"
#include "YetiOS_IconWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_IconWidget.h

* Description:
Base class for all icons
*************************************************************************/
UCLASS(Abstract, DisplayName = "Icon Widget")
class YETIOS_API UYetiOS_IconWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()
	
	FTimerHandle TimerHandle_DoubleClick;

private:

	/** Time to detect if double click or not. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS Icon Widget")
	float DoubleClickTime;

	/** Number of clicks by the user. */
	uint8 NumberOfClicks;

public:

	UYetiOS_IconWidget(const FObjectInitializer& ObjectInitializer);

public:

	/**
	* public UYetiOS_IconWidget::CheckForDoubleClick
	* Checks if the user has double clicked.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Icon Widget")
	void CheckForDoubleClick();

private:

	/**
	* private UYetiOS_IconWidget::Internal_OnDoubleClick
	* Called when a double click is detected.
	**/
	void Internal_OnDoubleClick();

protected:

	/** Event called when user double clicks. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Icon Widget", DisplayName = "OnDoubleClick")
	void K2_OnDoubleClick();
};
