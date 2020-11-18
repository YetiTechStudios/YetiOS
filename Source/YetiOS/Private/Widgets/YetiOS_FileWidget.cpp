// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_FileWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/YetiOS_FileBase.h"

UYetiOS_FileWidget* UYetiOS_FileWidget::CreateFileWidget(TSubclassOf<UYetiOS_FileWidget> FileWidgetClass, class UYetiOS_FileBase* InOwningFile)
{
	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(InOwningFile, 0);
	UYetiOS_FileWidget* ProxyWidget = CreateWidget<UYetiOS_FileWidget>(MyPlayerController, FileWidgetClass);
	ProxyWidget->OwningFile = InOwningFile;
	return ProxyWidget;
}
