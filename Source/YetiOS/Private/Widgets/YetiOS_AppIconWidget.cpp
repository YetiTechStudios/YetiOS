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
	DoubleClickTime = 0.2;
	BaseProgram = nullptr;
}

UYetiOS_AppIconWidget* UYetiOS_AppIconWidget::CreateProgramIconWidget(const UObject* WorldContextObject, class UYetiOS_BaseProgram* InCreatedProgram, FYetiOsError& OutErrorMessage)
{
	const UYetiOS_BaseDevice* MyDevice = InCreatedProgram->GetOwningOS()->GetOwningDevice();
	UYetiOS_Core* MyOS = MyDevice->GetOperatingSystem();
	if (MyOS && !MyOS->IsPendingKill())
	{
		APlayerController* MyController = UGameplayStatics::GetPlayerController(WorldContextObject, 0);
		UYetiOS_AppIconWidget* ProxyWidget = CreateWidget<UYetiOS_AppIconWidget>(MyController, InCreatedProgram->GetProgramIconWidgetClass());
		ProxyWidget->OwningOS = MyOS;
		ProxyWidget->BaseProgram = InCreatedProgram;
		ProxyWidget->ProgramClass = InCreatedProgram->GetClass();
		return ProxyWidget;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("Program_CreateWidgetErrorCode", "ERR_NO_OS");
	OutErrorMessage.ErrorException = LOCTEXT("Program_CreateWidgetErrorException", "Cannot create program widget.");
	OutErrorMessage.ErrorDetailedException = LOCTEXT("Program_CreateWidgetErrorDetailedException", "Failed to create program widget. An Operating System is required.");

	return nullptr;
}

void UYetiOS_AppIconWidget::CheckForDoubleClick()
{
	NumberOfClicks++;
	FTimerManager& MyTimerManager = GetWorld()->GetTimerManager();
	if (MyTimerManager.IsTimerActive(TimerHandle_DoubleClick) == false)
	{
		MyTimerManager.SetTimer(TimerHandle_DoubleClick, this, &UYetiOS_AppIconWidget::Internal_OnDoubleClick, DoubleClickTime, false);
	}
}

void UYetiOS_AppIconWidget::StartProgram(FYetiOsError& OutErrorMessage)
{
	if (BaseProgram != nullptr)
	{
		BaseProgram->StartProgram(OutErrorMessage);
	}
	else
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Program_StartErrorCode", "ERR_NO_PROGRAM_CLASS");
		OutErrorMessage.ErrorException = LOCTEXT("Program_StartErrorException", "Cannot start program.");
		OutErrorMessage.ErrorDetailedException = LOCTEXT("Program_StartErrorDetailedException", "Failed to start a program. Program reference was null.");

		FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, OutErrorMessage.ErrorException, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode, nullptr, EYetiOsNotificationType::TYPE_Error, 5.f);
		OwningOS->CreateOsNotification(NewNotification);
	}
}

void UYetiOS_AppIconWidget::Internal_OnDoubleClick()
{
	if (NumberOfClicks >= 2)
	{
		K2_OnDoubleClick();
	}

	NumberOfClicks = 0;
}

#undef LOCTEXT_NAMESPACE
