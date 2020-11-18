// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_FileWidget.generated.h"

/**
 * 
 */
UCLASS()
class YETIOS_API UYetiOS_FileWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()

private:
	
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS File Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileBase* OwningFile;

public:

	static UYetiOS_FileWidget* CreateFileWidget(TSubclassOf<UYetiOS_FileWidget> FileWidgetClass, class UYetiOS_FileBase* InOwningFile);
	
};
