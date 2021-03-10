// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_IconWidget.h"
#include "YetiOS_AppIconWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_AppIconWidget.h

* Description:
Widget that represents the app icon.
*************************************************************************/
UCLASS(Abstract, DisplayName = "Icon Widget")
class YETIOS_API UYetiOS_AppIconWidget : public UYetiOS_IconWidget
{
	GENERATED_BODY()
	
private:

	/** Program class to use as icon */
	UPROPERTY(EditAnywhere, Category = "Yeti OS App Icon Widget", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UYetiOS_BaseProgram> ProgramClass;

	/** Program that owns this widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* BaseProgram;
	
public:

	UYetiOS_AppIconWidget(const FObjectInitializer& ObjectInitializer);

	/**
	* public static UYetiOS_AppIconWidget::CreateProgramIconWidget
	* Creates the icon widget for given program.
	* @param InCreatedProgram [class UYetiOS_BaseProgram*] Program that will own this widget.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [UYetiOS_AppIconWidget*] Newly created icon widget.
	**/
	static UYetiOS_AppIconWidget* CreateProgramIconWidget(class UYetiOS_BaseProgram* InCreatedProgram, FYetiOsError& OutErrorMessage);
	
public:

	/**
	* public UYetiOS_AppIconWidget::StartProgram
	* Starts the owning program
	* @param OutErrorMessage [FYetiOsError&] Error message (if any)
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS App Icon Widget")	
	bool StartProgram(FYetiOsError& OutErrorMessage);

};
