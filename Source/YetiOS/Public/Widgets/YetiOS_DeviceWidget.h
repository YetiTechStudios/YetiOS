// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_DeviceWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_DeviceWidget.h

* Description:
Widget for the Base Device.
*************************************************************************/
UCLASS(Abstract, DisplayName = "Device Widget")
class YETIOS_API UYetiOS_DeviceWidget : public UUserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseDevice;

private:

	/** Device that owns this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Device Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseDevice* OwningDevice;

	/**
	* private static UYetiOS_DeviceWidget::Internal_CreateDeviceWidget
	* Creates the device widget for given device.
	* @param InDevice [UYetiOS_BaseDevice*] Device that will own this widget.
	* @return [UYetiOS_DeviceWidget*] Newly created device widget.
	**/
	static UYetiOS_DeviceWidget* Internal_CreateDeviceWidget(UYetiOS_BaseDevice* InDevice);
	
};
