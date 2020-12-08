// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "YetiOS_UserWidget.h"
#include "YetiOS_AppWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_AppWidget.h

* Description:
Widget that display the program.
*************************************************************************/
UCLASS(Abstract, DisplayName = "Application Widget")
class YETIOS_API UYetiOS_AppWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseProgram;

private:

	/** Program that owns this widget. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS App Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_BaseProgram* OwningProgram;

	/** File widget if this program supports files. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS App Widget", meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileWidget* FileWidget;

private:

	/**
	* private static UYetiOS_AppWidget::Internal_CreateAppWidget
	* Creates the application widget for the given program.
	* @param InParentProgram [const class UYetiOS_BaseProgram*] Program that owns this widget.
	* @return [UYetiOS_AppWidget*] Newly created application widget.
	**/
	static UYetiOS_AppWidget* Internal_CreateAppWidget(const class UYetiOS_BaseProgram* InParentProgram);

	/**
	* private UYetiOS_AppWidget::Internal_OnChangeVisibilityState
	* Called when the visibility of the owning program is changed.
	* @param InNewState [EYetiOsProgramVisibilityState] New visibility state.
	**/
	void Internal_OnChangeVisibilityState(EYetiOsProgramVisibilityState InNewState);

public:
		
	/**
	* public UYetiOS_AppWidget::DestroyProgramWidget
	* Destroys this application widget and removes it from parent.
	**/
	void DestroyProgramWidget();
	
	/**
	* public UYetiOS_AppWidget::SetFileWidget
	* Sets FileWidget to the given files widget.
	* @param InFile [class UYetiOS_FileBase*] FileWidget of this file will be set.
	**/
	void SetFileWidget(class UYetiOS_FileBase* InFile);

protected:

	/**
	* protected UYetiOS_AppWidget::K2_OnChangeVisibilityState
	* Event called when program changes its visibility state.
	* @param NewState [const EYetiOsProgramVisibilityState] New state
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "On Change Visibility State")	
	void K2_OnChangeVisibilityState(const EYetiOsProgramVisibilityState NewState);

	/**
	* protected UYetiOS_AppWidget::K2_OnFileWidgetSet
	* Event called when a valid (non-nullptr) file widget is set.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "On File Widget Set")	
	void K2_OnFileWidgetSet();

	/**
	* protected UYetiOS_AppWidget::K2_OnFileOpen
	* Event called when a new file is opened.
	* @param OpenedFile [class UYetiOS_FileBase *] Newly opened file or null.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS App Widget", DisplayName = "On File Open")
	void K2_OnFileOpen(class UYetiOS_FileBase* OpenedFile);
	
};
