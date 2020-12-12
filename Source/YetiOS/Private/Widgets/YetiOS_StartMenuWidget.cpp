// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_StartMenuWidget.h"
#include "Core/YetiOS_StartMenu.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

UYetiOS_StartMenuWidget* UYetiOS_StartMenuWidget::Internal_CreateStartMenuWidget(const UYetiOS_StartMenu* InStartMenu)
{
	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(InStartMenu, 0);
	UYetiOS_StartMenuWidget* ProxyWidget = CreateWidget<UYetiOS_StartMenuWidget>(MyPlayerController, InStartMenu->GetStartMenuWidgetClass());
	ProxyWidget->StartMenu = InStartMenu;
	ProxyWidget->SetOperatingSystem(InStartMenu->GetOwningOS());
	return ProxyWidget;
}
