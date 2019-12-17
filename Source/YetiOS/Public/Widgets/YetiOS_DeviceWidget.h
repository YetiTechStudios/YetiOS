// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_DeviceWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_DeviceWidget : public UUserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseDevice;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Device Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseDevice* OwningDevice;

	static UYetiOS_DeviceWidget* Internal_CreateDeviceWidget(UYetiOS_BaseDevice* InDevice);
	
};
