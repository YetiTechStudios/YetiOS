// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YetiOS_UserWidget.h"
#include "YetiOS_AppWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_AppWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseProgram;

private:

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS App Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* OwningProgram;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS App Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileWidget* FileWidget;

private:

	static UYetiOS_AppWidget* Internal_CreateAppWidget(const class UYetiOS_BaseProgram* InParentProgram);

	void Internal_OnChangeVisibilityState(EYetiOsProgramVisibilityState InNewState);

public:

	void DestroyProgramWidget();
	void SetFileWidget(class UYetiOS_FileBase* InFile);

protected:

	/* Event called when program changes its visibility state. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "On Change Visibility State")
	void K2_OnChangeVisibilityState(const EYetiOsProgramVisibilityState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "On File Widget Set")
	void K2_OnFileWidgetSet();

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "On File Open")
	void K2_OnFileOpen(class UYetiOS_FileBase* OpenedFile);
	
};
