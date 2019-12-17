// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Widgets/YetiOS_DeviceWidget.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Kismet/GameplayStatics.h"


UYetiOS_DeviceWidget* UYetiOS_DeviceWidget::Internal_CreateDeviceWidget(UYetiOS_BaseDevice* InDevice)
{
	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(InDevice->GetOuter(), 0);
	UYetiOS_DeviceWidget* ProxyWidget = CreateWidget<UYetiOS_DeviceWidget>(MyPlayerController, InDevice->GetDeviceWidgetClass());
	ProxyWidget->OwningDevice = InDevice;
	return ProxyWidget;
}
