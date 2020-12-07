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

	/** Peek to preview desktop. Similar to show desktop button at the end of the taskbar in Microsoft Windows. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Taskbar")
	uint8 bEnablePeekPreview : 1;

	/** Widget that represents the taskbar. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Taskbar")
	TSubclassOf<class UYetiOS_TaskbarWidget> TaskbarWidgetClass;

	/** Class that defines taskbar start menu. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Taskbar")
	TSubclassOf<class UYetiOS_StartMenu> StartMenuClass;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_TaskbarWidget* TaskbarWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_StartMenu* StartMenu;

	TWeakObjectPtr<UYetiOS_Core> OwningOS;	
	

protected:

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

	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")
	UYetiOS_TaskbarWidget* GetTaskbarWidget() const { return TaskbarWidget; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")
	class UYetiOS_Core* GetOwningOS() const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Taskbar")
	bool GetStartMenu(class UYetiOS_StartMenu*& OutStartMenu) const;

public:

	FORCEINLINE TSubclassOf<class UYetiOS_TaskbarWidget> GetTaskbarWidgetClass() const { return TaskbarWidgetClass; }	
	FORCEINLINE TSubclassOf<class UYetiOS_StartMenu> GeStartMenuClass() const { return StartMenuClass; }
};
