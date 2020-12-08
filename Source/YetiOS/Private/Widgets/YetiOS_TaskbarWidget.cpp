// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_TaskbarWidget.h"
#include "Core/YetiOS_Taskbar.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UYetiOS_TaskbarWidget* UYetiOS_TaskbarWidget::Internal_CreateTaskbarWidget(const UYetiOS_Taskbar* InTaskbar)
{
	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(InTaskbar, 0);
	UYetiOS_TaskbarWidget* ProxyWidget = CreateWidget<UYetiOS_TaskbarWidget>(MyPlayerController, InTaskbar->GetTaskbarWidgetClass());
	ProxyWidget->Taskbar = InTaskbar;
	ProxyWidget->SetOperatingSystem(InTaskbar->GetOwningOS());
	ProxyWidget->K2_OnCreate();
	return ProxyWidget;
}

void UYetiOS_TaskbarWidget::AddWindowToTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget)
{
	K2_OnWindowAdded(InWindowWidget);
}

void UYetiOS_TaskbarWidget::RemoveProgramFromTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget)
{
	K2_OnWindowRemoved(InWindowWidget);
}
