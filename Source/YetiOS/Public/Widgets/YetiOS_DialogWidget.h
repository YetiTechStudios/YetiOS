// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_UserWidget.h"
#include "YetiOS_Types.h"
#include "YetiOS_DialogWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogResult, EYetiOS_DialogResult, DialogResult);

UCLASS(Abstract)
class YETIOS_API UYetiOS_DialogWidget : public UYetiOS_UserWidget
{
	GENERATED_BODY()

	friend class UYetiOS_Core;
	
private:

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Dialog Widget", meta = (AllowPrivateAccess = "true"))
	EYetiOS_DialogType CurrentDialogType;

	UPROPERTY(BlueprintReadOnly, Category = "Yeti OS Dialog Widget", meta = (AllowPrivateAccess = "true"))
	uint8 bIsModal : 1;

	UPROPERTY()
	class UYetiOS_BaseDialogProgram* OwningProgram;

	static UYetiOS_DialogWidget* Internal_CreateDialogWidget(class UYetiOS_Core* InOS, class UYetiOS_BaseDialogProgram* InOwningProgram, TSubclassOf<UYetiOS_DialogWidget> InDialogWidgetClass, const bool bIsModalDialog, EYetiOS_DialogType InDialogType);

public:

	UYetiOS_DialogWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(BlueprintAssignable, Category = "Yeti OS Dialog Widget")
	FOnDialogResult OnDialogResult;

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

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Dialog Widget")
	void OnDialogClicked(EYetiOS_DialogResult DialogResult, const bool bCloseDialog = true);

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Dialog Widget", DisplayName = "On Set Title")
	void K2_OnSetTitle(const FText& InTitle);

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Dialog Widget", DisplayName = "On Set Message")
	void K2_OnSetMessage(const FText& InMessage);

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Dialog Widget", DisplayName = "On Show Dialog")
	void K2_OnShowDialog();

};
