// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "Widgets/YetiOS_AppIconWidget.h"
#include "Widgets/YetiOS_AppWidget.h"
#include "YetiOS_BaseProgram.generated.h"

/*************************************************************************
* File Information:
YetiOS_BaseProgram.h

* Description:
Base class for creating new programs.
*************************************************************************/
UCLASS(Abstract, Blueprintable)
class YETIOS_API UYetiOS_BaseProgram : public UObject
{
	GENERATED_BODY()
	
protected:

	/* Name of this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	FText ProgramName;

	/* A unique identifier for this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	FName ProgramIdentifier;

	/* An icon for this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	TSoftObjectPtr<UObject> ProgramIcon;

	/* How much space does this program consume once installed. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	float ProgramSpace;

	/* An icon widget that represents this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UYetiOS_AppIconWidget> ProgramIconWidgetClass;

	/* Widget associated with this program. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	TSubclassOf<UYetiOS_AppWidget> ProgramWidgetClass;

	/* If true, user can only run one instance of this program. If false, user can create as many instances as they want. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	uint8 bSingleInstanceOnly : 1;

	/* If true, then C++ can call K2_OnCreate function. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnCreate : 1;

	/* If true, then C++ can call K2_OnStart function. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnStart : 1;

	/* If true, then C++ can call K2_OnClose function. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnClose : 1;

	/* True if this program was installed as part of operating system installation. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsSystemInstalledProgram : 1;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_AppWidget* ProgramWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EYetiOsProgramVisibilityState CurrentVisibilityState;

	/* Operating system that owns this program. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	/* Process ID assigned to this program while running. Will be -1 (or INDEX_NONE in C++) if not running. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 ProcessID;
	
public:

	UYetiOS_BaseProgram();

	/**
	* public static UYetiOS_BaseProgram::CreateProgram
	* Creates the given program class inside given Operating System.
	* @param InOS [UYetiOS_Core*] Operating System this program belongs to.
	* @param ProgramClass [TSubclassOf<UYetiOS_BaseProgram>] Program class to create.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param bStartImmediately [const bool] If true, program will start immediately after creating.
	* @param bIsOsPredefinedProgram [const bool] If true, then consider this program was installed with the Operating system.
	* @return [UYetiOS_BaseProgram*] Newly created program. Will be nullptr if InOS is not valid.
	**/
	static UYetiOS_BaseProgram* CreateProgram(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_BaseProgram> ProgramClass, FYetiOsError& OutErrorMessage, const bool bStartImmediately = false, const bool bIsOsPredefinedProgram = false);
	
	/**
	* virtual public UYetiOS_BaseProgram::StartProgram
	* Start this program and add to running list in Operating System.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const bool] True if the program was started successfully.
	**/
	virtual const bool StartProgram(FYetiOsError& OutErrorMessage);

	/**
	* virtual public UYetiOS_BaseProgram::CloseProgram
	* Close this program.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Program")
	virtual void CloseProgram(FYetiOsError& OutErrorMessage);

	// #TERMINALPLUGIN TODO Not implemented yet
	//UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Program") 
	virtual bool ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState);

	/**
	* public UYetiOS_BaseProgram::GetProgramWidget const
	* Returns the widget that was created using Program Widget Class.
	* @return [class UYetiOS_AppWidget*] Reference to program widget.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")	
	UYetiOS_AppWidget* GetProgramWidget() const { return ProgramWidget; }

	/**
	* public UYetiOS_BaseProgram::GetOwningOS const
	* Returns the Operating System this program belongs to.
	* @return [class UYetiOS_Core*] Owning OS.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")	
	inline class UYetiOS_Core* GetOwningOS() const { return OwningOS; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")
	inline int32 GetProcessID() const { return ProcessID; }

protected:
		
	/**
	* protected UYetiOS_BaseProgram::K2_OnCreate
	* Event called when this program is created. @See CreateProgram() function.
	* NOTE: This event will ONLY be called if bCanCallOnCreate is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "OnCreate")	
	void K2_OnCreate();

	/**
	* protected UYetiOS_BaseProgram::K2_OnStart
	* Event called when program starts successfully. @See StartProgram() function.
	* NOTE: This event will ONLY be called if bCanCallOnStart is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "OnStart")	
	void K2_OnStart();

	/**
	* protected UYetiOS_BaseProgram::K2_OnClose
	* Event called when program closes. @See CloseProgram() function.
	* NOTE: This event will ONLY be called if bCanCallOnClose is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "OnClose")	
	void K2_OnClose();
	
public:

	FORCEINLINE UObject* GetProgramIcon() const { return ProgramIcon.Get(); }
	FORCEINLINE const bool IsSingleInstanceProgram() const { return bSingleInstanceOnly; }
	FORCEINLINE const float GetProgramSpace() const { return ProgramSpace; }
	FORCEINLINE TSubclassOf<UYetiOS_AppIconWidget> GetProgramIconWidgetClass() const { return ProgramIconWidgetClass; }
	FORCEINLINE TSubclassOf<UYetiOS_AppWidget> GetProgramWidgetClass() const { return ProgramWidgetClass; }
	FORCEINLINE const FText GetProgramName() const { return ProgramName; }
	FORCEINLINE const FName GetProgramIdentifierName() const { return ProgramIdentifier; }
	FORCEINLINE const bool IsSystemInstalledProgram() const { return bIsSystemInstalledProgram; }
};
