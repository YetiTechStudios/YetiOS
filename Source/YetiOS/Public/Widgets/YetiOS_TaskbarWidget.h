// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_TaskbarWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_TaskbarWidget.h

* Description:
Widget that represents the Operating System task bar.
*************************************************************************/
UCLASS(Abstract, DisplayName = "Taskbar Widget")
class YETIOS_API UYetiOS_TaskbarWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()

	friend class UYetiOS_Taskbar;
	
private:

	/** Weak pointer to Taskbar object */
	TWeakObjectPtr<const class UYetiOS_Taskbar> Taskbar;

	/**
	* private static UYetiOS_TaskbarWidget::Internal_CreateTaskbarWidget
	* Creates a new task bar widget
	* @param InTaskbar [const UYetiOS_Taskbar*] Taskbar that owns this widget.
	* @return [UYetiOS_TaskbarWidget*] Newly created task bar widget.
	**/
	static UYetiOS_TaskbarWidget* Internal_CreateTaskbarWidget(const UYetiOS_Taskbar* InTaskbar);

public:

	/**
	* public UYetiOS_TaskbarWidget::AddWindowToTaskbar
	* Adds the given draggable window to taskbar.
	* @param InWindowWidget [class UYetiOS_DraggableWindowWidget*] Draggable window widget.
	**/
	void AddWindowToTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget);

	/**
	* public UYetiOS_TaskbarWidget::RemoveProgramFromTaskbar
	* Remove the given draggable window from taskbar.
	* @param InWindowWidget [class UYetiOS_DraggableWindowWidget*] Draggable window widget
	**/
	void RemoveProgramFromTaskbar(class UYetiOS_DraggableWindowWidget* InWindowWidget);

protected:

	/**
	* protected UYetiOS_TaskbarWidget::K2_OnWindowAdded
	* Event called when a draggable window was added.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Taskbar Widget", DisplayName = "On Program Added")	
	void K2_OnWindowAdded(class UYetiOS_DraggableWindowWidget* InWindowWidget);

	/**
	* protected UYetiOS_TaskbarWidget::K2_OnWindowRemoved
	* Event called when a draggable window was removed.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Taskbar Widget", DisplayName = "On Program Removed")	
	void K2_OnWindowRemoved(class UYetiOS_DraggableWindowWidget* InWindowWidget);

	/**
	* protected UYetiOS_TaskbarWidget::K2_OnCreate
	* Event called when this task bar is created.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS Taskbar Widget", DisplayName = "On Create")	
	void K2_OnCreate();
};
