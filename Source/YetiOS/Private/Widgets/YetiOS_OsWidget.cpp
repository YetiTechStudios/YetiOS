// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_OsWidget.h"
#include "Core/YetiOS_Core.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


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
	K2_OnBeginLoadingOS();
}

void UYetiOS_OsWidget::BeginShutdownOS()
{
	K2_OnBeginShuttingdownOS();
}

void UYetiOS_OsWidget::OnBatteryLevelChanged(const float& CurrentBatteryLevel)
{
	K2_OnBatteryLevelChanged(CurrentBatteryLevel);
}

void UYetiOS_OsWidget::OnLowBatteryWarningReceived(const bool bIsLowBattery)
{
	K2_OnLowBatteryWarningReceived(bIsLowBattery);
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
	const float DeviceScore = OwningDevice->GetDeviceScore(true);
	return UKismetMathLibrary::MapRangeClamped(DeviceScore, 0.f, 1.f, InMin, InMax);
}
