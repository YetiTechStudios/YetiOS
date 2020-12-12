// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_IconWidget.h"

UYetiOS_IconWidget::UYetiOS_IconWidget(const FObjectInitializer& ObjectInitializer)
{
	DoubleClickTime = 0.5;
}

void UYetiOS_IconWidget::CheckForDoubleClick()
{
	NumberOfClicks++;
	FTimerManager& MyTimerManager = GetWorld()->GetTimerManager();
	if (MyTimerManager.IsTimerActive(TimerHandle_DoubleClick) == false)
	{
		MyTimerManager.SetTimer(TimerHandle_DoubleClick, this, &UYetiOS_AppIconWidget::Internal_OnDoubleClick, DoubleClickTime, false);
	}
}

void UYetiOS_IconWidget::Internal_OnDoubleClick()
{
	if (NumberOfClicks >= 2)
	{
		K2_OnDoubleClick();
	}

	NumberOfClicks = 0;
}
