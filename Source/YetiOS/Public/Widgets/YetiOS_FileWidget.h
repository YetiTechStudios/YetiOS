// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_FileWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_FileWidget.h

* Description:
Widget that represents the actual file.
*************************************************************************/
UCLASS(hidedropdown, DisplayName = "File Widget")
class YETIOS_API UYetiOS_FileWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()

private:
	
	/** File that owns this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS File Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileBase* OwningFile;

public:

	/**
	* public static UYetiOS_FileWidget::CreateFileWidget
	* Creates file widget.
	* @param FileWidgetClass [TSubclassOf<UYetiOS_FileWidget>] Widget class to use.
	* @param InOwningFile [class UYetiOS_FileBase*] File that owns this widget.
	* @return [UYetiOS_FileWidget*] Newly created file widget.
	**/
	static UYetiOS_FileWidget* CreateFileWidget(TSubclassOf<UYetiOS_FileWidget> FileWidgetClass, class UYetiOS_FileBase* InOwningFile);
	
};
