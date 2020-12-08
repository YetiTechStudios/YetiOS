// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Taskbar.generated.h"


UCLASS(Abstract, Blueprintable, DisplayName = "Operating System Taskbar")
class YETIOS_API UYetiOS_Taskbar : public UObject
{
	GENERATED_BODY()

	friend class UYetiOS_Core;

private:

	/** Peek to preview desktop. Similar to show desktop button at the end of the task bar in Microsoft Windows. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Taskbar")
	uint8 bEnablePeekPreview : 1;

	/** Widget that represents the task bar. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Taskbar")
	TSubclassOf<class UYetiOS_TaskbarWidget> TaskbarWidgetClass;

	/** Class that defines task bar start menu. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Taskbar")
	TSubclassOf<class UYetiOS_StartMenu> StartMenuClass;

	/** Reference to the task bar widget. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_TaskbarWidget* TaskbarWidget;

	/** Reference to the Start Menu asset. Only valid if Start Menu Class is set. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_StartMenu* StartMenu;

	/** Weak pointer to the Operating System that owns this task bar. */
	TWeakObjectPtr<UYetiOS_Core> OwningOS;	
	

protected:

	/**
	* protected static UYetiOS_Taskbar::CreateTaskbar
	* Creates the task bar for the given Operating System.
	* @param InCore [class UYetiOS_Core*] Target operating system.
	* @return [UYetiOS_Taskbar*] Reference to the task bar that was created.
	**/
	static UYetiOS_Taskbar* CreateTaskbar(class UYetiOS_Core* InCore);

public:

	/**
	* public UYetiOS_Taskbar::PeekDesktop
	* Peek preview desktop feature. Similar to show desktop button at the end of the taskbar in Microsoft Windows.
	* @param bEnable [const bool] Enable or disable peek preview.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Taskbar")
	void PeekDesktop(const bool bEnable);

	/**
	* public UYetiOS_Taskbar::IsPeekPreviewEnabled const
	* Returns true if peek preview options is enabled.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")
	bool IsPeekPreviewEnabled() const { return bEnablePeekPreview; }

	/**
	* public UYetiOS_Taskbar::GetTaskbarWidget const
	* Returns the task bar widget.
	* @return [UYetiOS_TaskbarWidget*] Task bar widget.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")	
	UYetiOS_TaskbarWidget* GetTaskbarWidget() const { return TaskbarWidget; }

	/**
	* public UYetiOS_Taskbar::GetOwningOS const
	* Returns the Operating System that owns this task bar.
	* @return [class UYetiOS_Core*] Operating System
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")
	class UYetiOS_Core* GetOwningOS() const;

	/**
	* public UYetiOS_Taskbar::GetStartMenu const
	* Returns the start menu (if any) of this task bar.
	* @param OutStartMenu [class UYetiOS_StartMenu*&] Outputs the start menu if available.
	* @return [bool] True if a valid start menu is available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")
	bool GetStartMenu(class UYetiOS_StartMenu*& OutStartMenu) const;

public:

	FORCEINLINE TSubclassOf<class UYetiOS_TaskbarWidget> GetTaskbarWidgetClass() const { return TaskbarWidgetClass; }	
	FORCEINLINE TSubclassOf<class UYetiOS_StartMenu> GeStartMenuClass() const { return StartMenuClass; }
};
