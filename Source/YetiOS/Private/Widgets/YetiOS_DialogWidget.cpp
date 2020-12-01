// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_DialogWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_BaseDialogProgram.h"

UYetiOS_DialogWidget::UYetiOS_DialogWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	CurrentDialogType = EYetiOS_DialogType::None;
	bIsModal = false;
}

UYetiOS_DialogWidget* UYetiOS_DialogWidget::Internal_CreateDialogWidget(class UYetiOS_Core* InOS, class UYetiOS_BaseDialogProgram* InOwningProgram, TSubclassOf<UYetiOS_DialogWidget> InDialogWidgetClass, const bool bIsModalDialog, EYetiOS_DialogType InDialogType)
{
	if (ensureMsgf(InOS != nullptr, TEXT("An operating system is required to create dialog widget.")))
	{
		APlayerController* MyController = UGameplayStatics::GetPlayerController(InOS, 0);
		UYetiOS_DialogWidget* ProxyDialog = CreateWidget<UYetiOS_DialogWidget>(MyController, InDialogWidgetClass);
		ProxyDialog->OwningProgram = InOwningProgram;
		ProxyDialog->bIsModal = bIsModalDialog;
		ProxyDialog->CurrentDialogType = InDialogType;
		ProxyDialog->SetOperatingSystem(InOS);
		ProxyDialog->K2_OnShowDialog();
		return ProxyDialog;
	}

	return nullptr;
}

void UYetiOS_DialogWidget::OnDialogClicked(EYetiOS_DialogResult DialogResult, const bool bCloseDialog /*= true*/)
{
	OnDialogResult.Broadcast(DialogResult);
	if (bCloseDialog && OwningProgram)
	{
		FYetiOsError OutError;
		OwningProgram->CloseProgram(OutError, false);
		OwningProgram = nullptr;
	}
}

bool UYetiOS_DialogWidget::CloseDialog()
{
	return UYetiOS_Core::CloseDialogWidget(OwningOS, this);	
}
