// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_Types.h"
#include "YetiOS_AppIconWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_AppIconWidget : public UUserWidget
{
	GENERATED_BODY()
	
	FTimerHandle TimerHandle_DoubleClick;
	
private:

	UPROPERTY(EditAnywhere, Category = "Yeti OS App Icon Widget")
	float DoubleClickTime;

	UPROPERTY(EditAnywhere, Category = "Yeti OS App Icon Widget", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UYetiOS_BaseProgram> ProgramClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* BaseProgram;

	uint8 NumberOfClicks;
	
public:

	UYetiOS_AppIconWidget(const FObjectInitializer& ObjectInitializer);

	static UYetiOS_AppIconWidget* CreateProgramIconWidget(const UObject* WorldContextObject, class UYetiOS_BaseProgram* InCreatedProgram, FYetiOsError& OutErrorMessage);
	
public:

	UFUNCTION(BlueprintCallable, Category = "Yeti OS App Icon")
	void CheckForDoubleClick();

	UFUNCTION(BlueprintCallable, Category = "Yeti OS App Icon")
	void StartProgram(FYetiOsError& OutErrorMessage);

private:

	void Internal_OnDoubleClick();
	
protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Icon", DisplayName = "OnDoubleClick")
	void K2_OnDoubleClick();
};
