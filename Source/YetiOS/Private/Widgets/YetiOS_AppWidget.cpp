// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_AppWidget.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Widgets/YetiOS_DraggableWindowWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/YetiOS_FileBase.h"


UYetiOS_AppWidget* UYetiOS_AppWidget::Internal_CreateAppWidget(const class UYetiOS_BaseProgram* InParentProgram)
{
	if (InParentProgram->GetProgramWidgetClass())
	{
		APlayerController* MyController = UGameplayStatics::GetPlayerController(InParentProgram, 0);
		UYetiOS_AppWidget* ProxyWidget = CreateWidget<UYetiOS_AppWidget>(MyController, InParentProgram->GetProgramWidgetClass());
		ProxyWidget->OwningProgram = const_cast<UYetiOS_BaseProgram*>(InParentProgram);
		ProxyWidget->SetOperatingSystem(InParentProgram->GetOwningOS());
		return ProxyWidget;
	}

	return nullptr;
}

void UYetiOS_AppWidget::Internal_OnChangeVisibilityState(EYetiOsProgramVisibilityState InNewState)
{
	K2_OnChangeVisibilityState(InNewState);
}

void UYetiOS_AppWidget::DestroyProgramWidget()
{
	OwningWindow = nullptr;
	OwningProgram = nullptr;
	OwningOS = nullptr;	
	RemoveFromParent();
}

void UYetiOS_AppWidget::OpenFile(class UYetiOS_FileBase* InFile)
{
	if (InFile)
	{
		if (FileWidget)
		{
			FileWidget->RemoveFromParent();
		}

		FileWidget = InFile->GetFileWidget();
		K2_OnFileWidgetSet();
	}

	K2_OnFileOpen(InFile);
}
