// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "YetiOS_AppWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_AppWidget : public UUserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseProgram;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS App Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* OwningProgram;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS App Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	UPROPERTY()
	class UYetiOS_DraggableWindowWidget* OwningWindow;

private:

	static UYetiOS_AppWidget* Internal_CreateAppWidget(const class UYetiOS_BaseProgram* InParentProgram);

private:

	void Internal_OnChangeVisibilityState(EYetiOsProgramVisibilityState InNewState);

public:

	void DestroyProgramWidget();
	void SetWindow(class UYetiOS_DraggableWindowWidget* InWindow) { OwningWindow = InWindow; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS App Widget")
	inline class UYetiOS_DraggableWindowWidget* GetOwningWindow() const { return OwningWindow; }

protected:

	/* Event called when program changes its visibility state. */
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "OnChangeVisibilityState")
	void K2_OnChangeVisibilityState(const EYetiOsProgramVisibilityState NewState);
	
};
