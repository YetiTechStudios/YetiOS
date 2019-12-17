// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_AppWidget.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Widgets/YetiOS_DraggableWindowWidget.h"
#include "Kismet/GameplayStatics.h"


UYetiOS_AppWidget* UYetiOS_AppWidget::Internal_CreateAppWidget(const class UYetiOS_BaseProgram* InParentProgram)
{
	APlayerController* MyController = UGameplayStatics::GetPlayerController(InParentProgram, 0);
	UYetiOS_AppWidget* ProxyWidget = CreateWidget<UYetiOS_AppWidget>(MyController, InParentProgram->GetProgramWidgetClass());
	ProxyWidget->OwningProgram = const_cast<UYetiOS_BaseProgram*>(InParentProgram);
	ProxyWidget->OwningOS = InParentProgram->GetOwningOS();
	return ProxyWidget;
}

void UYetiOS_AppWidget::Internal_OnChangeVisibilityState(EYetiOsProgramVisibilityState InNewState)
{
	K2_OnChangeVisibilityState(InNewState);
}

void UYetiOS_AppWidget::DestroyProgramWidget()
{
	OwningWindow->CloseWindow();
	OwningWindow = nullptr;
	OwningProgram = nullptr;
	OwningOS = nullptr;	
	RemoveFromParent();
}
