// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_OsWidget.h"
#include "Core/YetiOS_Core.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/YetiOS_SystemSettings.h"
#include "Runtime/Engine/Classes/Sound/SoundBase.h"


UYetiOS_OsWidget::UYetiOS_OsWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ZOrderCounter = 5;
}

UYetiOS_OsWidget* UYetiOS_OsWidget::Internal_CreateOsWidget(const UYetiOS_Core* OsCore)
{
	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(OsCore, 0);
	UYetiOS_OsWidget* ProxyOsWidget = CreateWidget<UYetiOS_OsWidget>(MyPlayerController, OsCore->GetOsWidgetClass());
	ProxyOsWidget->OwningOS = const_cast<UYetiOS_Core*>(OsCore);
	ProxyOsWidget->OwningDevice = OsCore->GetOwningDevice();
	return ProxyOsWidget;
}

void UYetiOS_OsWidget::StartOsInstallation(const float& InTimeToInstall)
{
	K2_OnInstallationStarted(InTimeToInstall);
}

void UYetiOS_OsWidget::FinishOsInstallation()
{
	K2_OnFinishInstallation();
}

void UYetiOS_OsWidget::BeginLoadOS()
{
	UYetiOS_SystemSettings* OsSystemSettings = OwningOS->GetSystemSettings();
	if (OsSystemSettings)
	{
		K2_OnThemeChanged(OsSystemSettings->GetCurrentTheme());
		OnThemeChangedDelegateHandle = OsSystemSettings->OnThemeModeChanged.AddUFunction(this, FName("K2_OnThemeChanged"));
	}
	K2_OnBeginLoadingOS();
}

void UYetiOS_OsWidget::BeginShutdownOS()
{
	UYetiOS_SystemSettings* OsSystemSettings = OwningOS->GetSystemSettings();
	if (OsSystemSettings)
	{
		OsSystemSettings->OnThemeModeChanged.Remove(OnThemeChangedDelegateHandle);
		OnThemeChangedDelegateHandle.Reset();
	}
	K2_OnBeginShuttingdownOS();
}

void UYetiOS_OsWidget::BeginRestartOS()
{
	K2_OnBeginRestartingOS();
}

void UYetiOS_OsWidget::OnBatteryLevelChanged(const float& CurrentBatteryLevel)
{
	K2_OnBatteryLevelChanged(CurrentBatteryLevel);
}

void UYetiOS_OsWidget::OnLowBatteryWarningReceived(const bool bIsLowBattery)
{
	K2_OnLowBatteryWarningReceived(bIsLowBattery);
}

void UYetiOS_OsWidget::AddTaskbarButton(class UYetiOS_DraggableWindowWidget* InWindowWidget)
{
	ensureAlwaysMsgf(false, TEXT("Deprecated since v2.0. Please use the new taskbar class."));
}

void UYetiOS_OsWidget::RemoveTaskbarButton(class UYetiOS_DraggableWindowWidget* InWindowWidget)
{
	ensureAlwaysMsgf(false, TEXT("Deprecated since v2.0. Please use the new taskbar class."));
}

void UYetiOS_OsWidget::ReceiveNotification(const FYetiOsNotification InNotification)
{
	K2_OnReceivedNotification(InNotification);
}

bool UYetiOS_OsWidget::ChangePassword(FYetiOsUser& InNewUser, const FText InNewPassword)
{
	if (OwningOS)
	{
		return OwningOS->ChangePassword(InNewUser, InNewPassword);
	}

	return false;
}

void UYetiOS_OsWidget::PlayNotificationSound(const FYetiOsNotification& InNotification, const float InVolume /*= 1.f*/)
{
	USoundBase* MySound = OwningOS->GetNotificationSound(InNotification);
	UGameplayStatics::PlaySound2D(this, MySound, InVolume);
}

const bool UYetiOS_OsWidget::SetActiveUser(FYetiOsUser InNewUser)
{
	if (OwningOS)
	{
		OwningOS->SetActiveUser(InNewUser);
		return true;
	}

	return false;
}

const float UYetiOS_OsWidget::GetDelayTime(const float InMin /*= 0.01*/, const float InMax /*= 0.2*/) const
{
	return FMath::RandRange(InMin, InMax);
}
