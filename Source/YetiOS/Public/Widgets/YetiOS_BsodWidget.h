// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_BsodWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_BsodWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS BSOD Widget", meta = (AllowPrivateAccess = "true"))
	FText FaultingModuleName;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS BSOD Widget", meta = (AllowPrivateAccess = "true"))
	FText ExceptionName;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS BSOD Widget", meta = (AllowPrivateAccess = "true"))
	FText DetailedExceptionName;
	
public:

	static UYetiOS_BsodWidget* CreateBsodWidget(const UObject* WorldContextObject, TSubclassOf<class UYetiOS_BsodWidget> InBsodWidgetClass, const FText InFaultingModuleName = FText::GetEmpty(), const FText InExceptionName = FText::GetEmpty(), const FText InDetailedException = FText::GetEmpty());

	UFUNCTION(BlueprintCallable, Category = "Yeti BSOD")
	static const FText GenerateRandomErrorCode();
};
