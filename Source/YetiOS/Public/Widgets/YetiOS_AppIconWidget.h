// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_IconWidget.h"
#include "YetiOS_AppIconWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_AppIconWidget : public UYetiOS_IconWidget
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditAnywhere, Category = "Yeti OS App Icon Widget", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UYetiOS_BaseProgram> ProgramClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* BaseProgram;
	
public:

	UYetiOS_AppIconWidget(const FObjectInitializer& ObjectInitializer);

	static UYetiOS_AppIconWidget* CreateProgramIconWidget(class UYetiOS_BaseProgram* InCreatedProgram, FYetiOsError& OutErrorMessage);
	
public:

	UFUNCTION(BlueprintCallable, Category = "Yeti OS App Icon Widget")
	void StartProgram(FYetiOsError& OutErrorMessage);

};
