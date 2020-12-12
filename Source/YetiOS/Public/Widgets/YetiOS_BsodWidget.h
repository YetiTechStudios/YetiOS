// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_BsodWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_BsodWidget.h

* Description:
Widget that represents BSOD (Blue Screen of Death)
*************************************************************************/
UCLASS(Abstract, DisplayName = "BSOD Widget")
class YETIOS_API UYetiOS_BsodWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:

	/** Module name */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS BSOD Widget", meta = (AllowPrivateAccess = "true"))
	FText FaultingModuleName;

	/** Exception name or title */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS BSOD Widget", meta = (AllowPrivateAccess = "true"))
	FText ExceptionName;

	/** Detailed exception. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS BSOD Widget", meta = (AllowPrivateAccess = "true"))
	FText DetailedExceptionName;
	
public:

	/**
	* public static UYetiOS_BsodWidget::CreateBsodWidget
	* Creates a BSOD widget from given class.
	* @param InBsodWidgetClass [TSubclassOf<class UYetiOS_BsodWidget>] BSOD widget class.
	* @param InFaultingModuleName [const FText] Module name.
	* @param InExceptionName [const FText] Exception or title.
	* @param InDetailedException [const FText] Detailed exception.
	* @return [UYetiOS_BsodWidget*] Newly created BSOD widget.
	**/
	static UYetiOS_BsodWidget* CreateBsodWidget(const UObject* WorldContextObject, TSubclassOf<class UYetiOS_BsodWidget> InBsodWidgetClass, const FText InFaultingModuleName = FText::GetEmpty(), const FText InExceptionName = FText::GetEmpty(), const FText InDetailedException = FText::GetEmpty());

	/**
	* public static UYetiOS_BsodWidget::GenerateRandomErrorCode
	* Generates a random error code.
	* @return [const FText] Error code.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Global")	
	static const FText GenerateRandomErrorCode();
};
