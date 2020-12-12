// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_Types.h"
#include "YetiOS_DialogWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogResult, EYetiOS_DialogResult, DialogResult);

/*************************************************************************
* File Information:
YetiOS_DialogWidget.h

* Description:
Widget that represents any kind of dialog. Example: MessageBox
*************************************************************************/
UCLASS(Abstract, DisplayName = "Dialog Widget")
class YETIOS_API UYetiOS_DialogWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()

	friend class UYetiOS_Core;
	
private:

	/** Current type of dialog. */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Dialog Widget", meta = (AllowPrivateAccess = "true"))
	EYetiOS_DialogType CurrentDialogType;

	/** Is this a modal dialog? @See https://ux.stackexchange.com/questions/12045/what-is-a-modal-dialog-window */
	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Dialog Widget", meta = (AllowPrivateAccess = "true"))
	uint8 bIsModal : 1;

	/** Dialog Program that owns this dialog widget. */
	UPROPERTY()
	class UYetiOS_BaseDialogProgram* OwningProgram;

	/**
	* private static UYetiOS_DialogWidget::Internal_CreateDialogWidget
	* Creates a new dialog widget.
	* @param InOS [class UYetiOS_Core*] Operating system to create this dialog for.
	* @param InOwningProgram [class UYetiOS_BaseDialogProgram*] Program that owns this widget.
	* @param InDialogWidgetClass [TSubclassOf<UYetiOS_DialogWidget>] Dialog widget class.
	* @param bIsModalDialog [const bool] True if this should be treated as a modal dialog.
	* @param InDialogType [EYetiOS_DialogType] Type of this dialog
	* @return [UYetiOS_DialogWidget*] Newly created dialog widget.
	**/
	static UYetiOS_DialogWidget* Internal_CreateDialogWidget(class UYetiOS_Core* InOS, class UYetiOS_BaseDialogProgram* InOwningProgram, TSubclassOf<UYetiOS_DialogWidget> InDialogWidgetClass, const bool bIsModalDialog, EYetiOS_DialogType InDialogType);

public:

	UYetiOS_DialogWidget(const FObjectInitializer& ObjectInitializer);

	/** Delegate called when a button on this dialog is clicked. */
	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Dialog Widget")
	FOnDialogResult OnDialogResult;

	/**
	* public UYetiOS_DialogWidget::CloseDialog
	* Closes this dialog.
	* @return [bool] True if the dialog was closed.
	**/
	bool CloseDialog();

	/**
	* public UYetiOS_DialogWidget::IsModalDialog const
	* Checks if this dialog is a modal dialog.
	* @return [bool] True if this is a modal dialog.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Dialog Widget")	
	bool IsModalDialog() const { return bIsModal; }

	/**
	* public UYetiOS_DialogWidget::GetDialogType const
	* Returns the type of this dialog.
	* @return [EYetiOS_DialogType] An enum that defines the type of this dialog.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Dialog Widget")	
	EYetiOS_DialogType GetDialogType() const { return CurrentDialogType; }

protected:

	/**
	* protected UYetiOS_DialogWidget::OnDialogClicked
	* Indicates that dialog has been interacted with by the user.
	* @param DialogResult [EYetiOS_DialogResult] Type of result the user clicked.
	* @param bCloseDialog [const bool] True if the dialog should be closed. Generally this should always be true.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Dialog Widget")	
	void OnDialogClicked(EYetiOS_DialogResult DialogResult, const bool bCloseDialog = true);

	/**
	* protected UYetiOS_DialogWidget::K2_OnSetTitle
	* Sets the title for this dialog
	* @param InTitle [const FText&] New title to use.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Dialog Widget", DisplayName = "On Set Title")	
	void K2_OnSetTitle(const FText& InTitle);

	/**
	* protected UYetiOS_DialogWidget::K2_OnSetMessage
	* Sets the message for this dialog
	* @param InMessage [const FText&] New message to use.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Dialog Widget", DisplayName = "On Set Message")	
	void K2_OnSetMessage(const FText& InMessage);

	/**
	* protected UYetiOS_DialogWidget::K2_OnShowDialog
	* Event called when this dialog is shown.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Dialog Widget", DisplayName = "On Show Dialog")	
	void K2_OnShowDialog();

};
