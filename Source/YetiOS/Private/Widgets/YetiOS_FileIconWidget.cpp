// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_FileIconWidget.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Core/YetiOS_FileBase.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_FileIconWidget::UYetiOS_FileIconWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	BaseFile = nullptr;
}

UYetiOS_FileIconWidget* UYetiOS_FileIconWidget::CreateFileIconWidget(class UYetiOS_FileBase* InFile, FYetiOsError& OutErrorMessage)
{
	const UYetiOS_BaseDevice* MyDevice = InFile->GetParentDirectory()->GetOwningOS()->GetOwningDevice();
	UYetiOS_Core* MyOS = MyDevice->GetOperatingSystem();
	if (MyOS && !MyOS->IsPendingKill())
	{
		APlayerController* MyController = UGameplayStatics::GetPlayerController(InFile, 0);
		UYetiOS_FileIconWidget* ProxyWidget = CreateWidget<UYetiOS_FileIconWidget>(MyController, InFile->GetFileIconWidgetClass());
		ProxyWidget->SetOperatingSystem(MyOS);
		ProxyWidget->BaseFile = InFile;
		ProxyWidget->FileClass = InFile->GetClass();
		return ProxyWidget;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("FileIcon_CreateWidgetErrorCode", "ERR_NO_OS");
	OutErrorMessage.ErrorException = LOCTEXT("FileIcon_CreateWidgetErrorException", "Cannot create file icon widget.");
	OutErrorMessage.ErrorDetailedException = LOCTEXT("FileIcon_CreateWidgetErrorDetailedException", "Failed to create file icon widget. An Operating System is required.");

	return nullptr;
}

void UYetiOS_FileIconWidget::AssociatedProgramInstalled()
{
	K2_OnAssociatedProgramInstalled();
}

bool UYetiOS_FileIconWidget::OpenFile(FYetiOsError& OutErrorMessage)
{
	if (BaseFile)
	{
		return BaseFile->OpenFile(OutErrorMessage);
	}

	OutErrorMessage.ErrorCode = LOCTEXT("File_StartErrorCode", "ERR_NO_FILE_CLASS");
	OutErrorMessage.ErrorException = LOCTEXT("File_StartErrorException", "Cannot open file.");
	OutErrorMessage.ErrorDetailedException = LOCTEXT("File_StartErrorDetailedException", "Failed to open file. File reference was null.");

	FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, OutErrorMessage.ErrorException, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode, nullptr, EYetiOsNotificationType::TYPE_Error, 5.f);
	OwningOS->CreateOsNotification(NewNotification);
	return false;
}

bool UYetiOS_FileIconWidget::IsAssociatedProgramInstalled() const
{
	return BaseFile->IsAssociatedProgramInstalled();
}

#undef LOCTEXT_NAMESPACE
