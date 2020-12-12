// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/YetiOS_IconWidget.h"
#include "YetiOS_FileIconWidget.generated.h"

/*************************************************************************
* File Information:
YetiOS_FileIconWidget.h

* Description:
Widget for file icon.
*************************************************************************/
UCLASS(Abstract, DisplayName = "File Icon Widget")
class YETIOS_API UYetiOS_FileIconWidget : public UYetiOS_IconWidget
{
	GENERATED_BODY()

private:

	/** Class that this icon belongs to. */
	UPROPERTY(EditAnywhere, Category = "Yeti OS File Icon Widget", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UYetiOS_FileBase> FileClass;

	/** File that owns this widget. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileBase* BaseFile;

public:

	UYetiOS_FileIconWidget(const FObjectInitializer& ObjectInitializer);

	/**
	* public static UYetiOS_FileIconWidget::CreateFileIconWidget
	* Creates file icon widget.
	* @param InFile [class UYetiOS_FileBase*] File that owns this widget.
	* @param OutErrorMessage [FYetiOsError] Error message (if any).
	* @return [UYetiOS_FileIconWidget*] Newly created file icon widget.
	**/
	static UYetiOS_FileIconWidget* CreateFileIconWidget(class UYetiOS_FileBase* InFile, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_FileIconWidget::AssociatedProgramInstalled
	* Function called when Associated program is installed.
	**/
	void AssociatedProgramInstalled();
	
	/**
	* public UYetiOS_FileIconWidget::OpenFile
	* Opens the owning file.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [bool] True if file was opened.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS File Icon Widget")	
	bool OpenFile(FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_FileIconWidget::IsAssociatedProgramInstalled const
	* Checks if this associated program of the file is installed.
	* @return [bool] True of program is installed.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File Icon Widget")	
	bool IsAssociatedProgramInstalled() const;

protected:

	/**
	* protected UYetiOS_FileIconWidget::K2_OnAssociatedProgramInstalled
	* Event called when associated program is installed on the Operating System.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS File Icon Widget", DisplayName = "On Associated Program Installed")	
	void K2_OnAssociatedProgramInstalled();
};
