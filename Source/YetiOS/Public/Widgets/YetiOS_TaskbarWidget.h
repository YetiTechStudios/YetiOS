// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_TaskbarWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class YETIOS_API UYetiOS_TaskbarWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()

	friend class UYetiOS_Taskbar;
	
private:

	TWeakObjectPtr<const class UYetiOS_Taskbar> Taskbar;

	static UYetiOS_TaskbarWidget* Internal_CreateTaskbarWidget(const UYetiOS_Taskbar* InTaskbar);

public:

	void AddProgramToTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget);
	void RemoveProgramFromTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget);

protected:

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Taskbar", DisplayName = "On Program Added")
	void K2_OnWindowAdded(class UYetiOS_DraggableWindowWidget* InWindowWidget);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Taskbar", DisplayName = "On Program Removed")
	void K2_OnWindowRemoved(class UYetiOS_DraggableWindowWidget* InWindowWidget);
};
