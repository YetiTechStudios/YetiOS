// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_AppIconWidget.h"
#include "Core/YetiOS_Core.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Engine/Public/TimerManager.h"
#include "Kismet/GameplayStatics.h"


#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_AppIconWidget::UYetiOS_AppIconWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseProgram = nullptr;
}

UYetiOS_AppIconWidget* UYetiOS_AppIconWidget::CreateProgramIconWidget(class UYetiOS_BaseProgram* InCreatedProgram, FYetiOsError& OutErrorMessage)
{
	UYetiOS_Core* MyOS = InCreatedProgram->GetOwningOS();
	if (MyOS && !MyOS->IsPendingKill())
	{
		if (InCreatedProgram->GetProgramIconWidgetClass() == nullptr)
		{
			return nullptr;
		}

		APlayerController* MyController = UGameplayStatics::GetPlayerController(InCreatedProgram, 0);
		UYetiOS_AppIconWidget* ProxyWidget = CreateWidget<UYetiOS_AppIconWidget>(MyController, InCreatedProgram->GetProgramIconWidgetClass());
		ProxyWidget->SetOperatingSystem(MyOS);
		ProxyWidget->BaseProgram = InCreatedProgram;
		ProxyWidget->ProgramClass = InCreatedProgram->GetClass();
		InCreatedProgram->AddProgramIconWidget(ProxyWidget);
		return ProxyWidget;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("Program_CreateWidgetErrorCode", "ERR_NO_OS");
	OutErrorMessage.ErrorException = LOCTEXT("Program_CreateWidgetErrorException", "Cannot create program icon widget.");
	OutErrorMessage.ErrorDetailedException = LOCTEXT("Program_CreateWidgetErrorDetailedException", "Failed to create program icon widget. An Operating System is required.");

	return nullptr;
}

bool UYetiOS_AppIconWidget::StartProgram(FYetiOsError& OutErrorMessage)
{
	if (BaseProgram)
	{
		return OwningOS->StartProgram(BaseProgram->GetProgramIdentifierName(), OutErrorMessage);
	}
	
	OutErrorMessage.ErrorCode = LOCTEXT("Program_StartErrorCode", "ERR_NO_PROGRAM_CLASS");
	OutErrorMessage.ErrorException = LOCTEXT("Program_StartErrorException", "Cannot start program.");
	OutErrorMessage.ErrorDetailedException = LOCTEXT("Program_StartErrorDetailedException", "Failed to start a program. Program reference was null.");

	FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, OutErrorMessage.ErrorException, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode, nullptr, EYetiOsNotificationType::TYPE_Error, 5.f);
	OwningOS->CreateOsNotification(NewNotification);

	return false;
}

#undef LOCTEXT_NAMESPACE
