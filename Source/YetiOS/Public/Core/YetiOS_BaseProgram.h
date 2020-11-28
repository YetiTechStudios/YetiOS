// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "Widgets/YetiOS_AppIconWidget.h"
#include "Widgets/YetiOS_AppWidget.h"
#include "YetiOS_BaseProgram.generated.h"

UENUM(BlueprintType)
enum class EProgramSaveMethod : uint8
{
	/* Save automatically when Operating System is shutting down. */
	SaveOnOperatingSystemShutdown,

	/* Save automatically whenever this program is closed. Also saves on operating system shutdown since the program exits at shutdown. */
	SaveOnExit,

	/* No automatic saving. Player has to manually save settings using SaveSettinsg() method. */
	SaveManually,

	/* Do not save at all. */
	DoNotSave
};

/*************************************************************************
* File Information:
YetiOS_BaseProgram.h

* Description:
Base class for creating new programs.
*************************************************************************/
UCLASS(hidedropdown, Blueprintable, DisplayName = "Base Program")
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

	/** Version identifier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	FYetiOS_Version ProgramVersion;

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

	/* An optional class where you can save custom settings for this program. For example: Customizable options for a browser. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	TSubclassOf<class UYetiOS_ProgramSettings> SettingsClass;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	EProgramSaveMethod SaveMethod;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program", meta = (InlineEditConditionToggle))
	uint8 bOverrideWindowSize : 1;

	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program", meta = (EditCondition = "bOverrideWindowSize"))
	FVector2D OverrideWindowSize;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	uint8 bAddToDesktop : 1;

	/* If true, user can only run one instance of this program. If false, user can create as many instances as they want. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	uint8 bSingleInstanceOnly : 1;

	/* If true, then this program can run on portable device. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanRunOnPortableDevice : 1;

	/* If true, then C++ can call K2_OnCreate function. Implement OnCreate event in Blueprint Graph */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnCreate : 1;

	/* If true, then C++ can call K2_OnInstall function. Implement OnInstall event in Blueprint Graph */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnInstall : 1;

	/* If true, then C++ can call K2_OnStart function. Implement OnStart event in Blueprint Graph. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnStart : 1;

	/* If true, then C++ can call K2_OnClose function. Implement OnClose event in Blueprint Graph. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnClose : 1;

	/* True if this program was installed as part of operating system installation. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsSystemInstalledProgram : 1;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_AppWidget* ProgramWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileBase* CurrentFileOpened;

	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EYetiOsProgramVisibilityState CurrentVisibilityState;

	/* Operating system that owns this program. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_ProgramSettings* ProgramSettings;

	/* Process ID assigned to this program while running. Will be -1 (or INDEX_NONE in C++) if not running. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 ProcessID;

	UPROPERTY()
	UYetiOS_AppIconWidget* ProgramIconWidget;
	
public:

	UYetiOS_BaseProgram();

	/**
	* public static UYetiOS_BaseProgram::CreateProgram
	* Creates the given program class inside given Operating System.
	* @param InOS [UYetiOS_Core*] Operating System this program belongs to.
	* @param ProgramClass [TSubclassOf<UYetiOS_BaseProgram>] Program class to create.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param bInstalledWithOS [const bool] If true, then consider this program was installed with the Operating system.
	* @return [UYetiOS_BaseProgram*] Newly created program. Will be nullptr if InOS is not valid.
	**/
	static UYetiOS_BaseProgram* CreateProgram(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_BaseProgram> ProgramClass, FYetiOsError& OutErrorMessage, const bool bInstalledWithOS = false);
	
private:

	static UYetiOS_BaseProgram* Internal_StartProgram(UYetiOS_BaseProgram* Program, FYetiOsError& OutErrorMessage);

public:

	virtual void ProgramInstalled();

	/**
	* virtual public UYetiOS_BaseProgram::StartProgram
	* Start this program and add to running list in Operating System.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const bool] True if the program was started successfully.
	**/
	virtual const bool StartProgram(FYetiOsError& OutErrorMessage);

	virtual const bool StartProgram(UYetiOS_BaseProgram*& OutProgram, FYetiOsError& OutErrorMessage);

	/**
	* virtual public UYetiOS_BaseProgram::ChangeVisibilityState
	* Changes visibility state of this program. Used to minimize, maximize etc.
	* @param InNewState [const EYetiOsProgramVisibilityState] New state to switch to.
	* @return [bool] True if the state was changed successfully.
	**/
	virtual bool ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState);

	/**
	* public UYetiOS_BaseProgram::AddProgramIconWidget
	* Add the icon widget to this program.
	* @param InIconWidget [class UYetiOS_AppIconWidget*] Widget to set.
	**/
	void AddProgramIconWidget(class UYetiOS_AppIconWidget* InIconWidget);

	/**
	* virtual public UYetiOS_BaseProgram::CloseProgram
	* Close this program.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Program")
	virtual void CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown = false);

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Program")
	virtual bool OpenFile(class UYetiOS_FileBase* InFileToOpen);

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

	/**
	* public UYetiOS_BaseProgram::IsRunning const
	* Checks if this program is running.
	* @return [bool] True if this program is currently running.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")	
	bool IsRunning() const { return ProcessID != INDEX_NONE; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")
	class UYetiOS_AppIconWidget* GetProgramIconWidget() const { return ProgramIconWidget; }

	/**
	* public UYetiOS_BaseProgram::GetProgramVersion const
	* Returns the program version.
	* @return [FYetiOS_Version] Program version struct.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")	
	FYetiOS_Version GetProgramVersion() const { return ProgramVersion; }

private:

	bool Internal_LoadProgramSettings();

protected:
		
	/**
	* protected UYetiOS_BaseProgram::K2_OnCreate
	* Event called when this program is created. @See CreateProgram() function.
	* NOTE: This event will ONLY be called if bCanCallOnCreate is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "On Create")	
	void K2_OnCreate();

	/**
	* protected UYetiOS_BaseProgram::K2_OnInstalled
	* Event called when this program is installed.
	* NOTE: This event will ONLY be called if bCanCallOnInstall is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "On Installed")	
	void K2_OnInstall();

	/**
	* protected UYetiOS_BaseProgram::K2_OnStart
	* Event called when program starts successfully. @See StartProgram() function.
	* NOTE: This event will ONLY be called if bCanCallOnStart is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "On Start")	
	void K2_OnStart();

	/**
	* protected UYetiOS_BaseProgram::K2_OnClose
	* Event called when program closes. @See CloseProgram() function.
	* NOTE: This event will ONLY be called if bCanCallOnClose is true!
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "On Close")	
	void K2_OnClose();

	/**
	* protected UYetiOS_BaseProgram::K2_OnSettingsLoad
	* Event called when program settings are loaded. Use Get Program Settings node to access and load your settings manually.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "On Load Settings")	
	void K2_OnSettingsLoad();

	/**
	* protected UYetiOS_BaseProgram::K2_OnOpenFile
	* Event called when a file is opened.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Program", DisplayName = "On Open File")	
	void K2_OnOpenFile();
	
public:

	FORCEINLINE UYetiOS_ProgramSettings* GetProgramSettings() const { return ProgramSettings; }
	FORCEINLINE UObject* GetProgramIcon() const { return ProgramIcon.LoadSynchronous(); }
	FORCEINLINE const bool IsSingleInstanceProgram() const { return bSingleInstanceOnly; }
	FORCEINLINE const bool CanAddToDesktop() const { return bAddToDesktop; }
	FORCEINLINE const float GetProgramSpace() const { return ProgramSpace; }
	FORCEINLINE TSubclassOf<UYetiOS_AppIconWidget> GetProgramIconWidgetClass() const { return ProgramIconWidgetClass; }
	FORCEINLINE TSubclassOf<UYetiOS_AppWidget> GetProgramWidgetClass() const { return ProgramWidgetClass; }
	FORCEINLINE const FText GetProgramName() const { return ProgramName; }
	FORCEINLINE const FName GetProgramIdentifierName() const { return ProgramIdentifier; }
	FORCEINLINE const bool IsSystemInstalledProgram() const { return bIsSystemInstalledProgram; }
	FORCEINLINE const bool CanSaveSettings() const { return ProgramSettings != nullptr && SaveMethod != EProgramSaveMethod::DoNotSave; }
	FORCEINLINE const EYetiOsProgramVisibilityState GetCurrentVisibilityState() const { return CurrentVisibilityState; }
};
