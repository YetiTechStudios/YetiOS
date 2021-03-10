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
	/** Save automatically when Operating System is shutting down. */
	SaveOnOperatingSystemShutdown,

	/** Save automatically whenever this program is closed. Also saves on operating system shutdown since the program exits at shutdown. */
	SaveOnExit,

	/** No automatic saving. Player has to manually save settings using SaveSettinsg() method. */
	SaveManually,

	/** Do not save at all. */
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

	/** Name of this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	FText ProgramName;

	/** A unique identifier for this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	FName ProgramIdentifier;

	/** Version identifier. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	FYetiOS_Version ProgramVersion;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program", meta = (InlineEditConditionToggle))
	uint8 bRequireMinimumOperatingSystemVersion : 1;

	/** If specified, this program will not install or run on an Operating System less than this version. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true", EditCondition = "bRequireMinimumOperatingSystemVersion"))
	FYetiOS_Version MinimumOperatingSystemVersionRequired;

	/** An icon for this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true", DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	TSoftObjectPtr<UObject> ProgramIcon;

	/** How much space (in Megabyte) does this program consume once installed. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	float ProgramSpaceInMB;

	/** An icon widget that represents this program. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UYetiOS_AppIconWidget> ProgramIconWidgetClass;

	/** Widget associated with this program. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	TSubclassOf<UYetiOS_AppWidget> ProgramWidgetClass;

	/** An optional class where you can save custom settings for this program. For example: Customizable options for a browser. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	TSubclassOf<class UYetiOS_ProgramSettings> SettingsClass;

	/** How to save this program. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	EProgramSaveMethod SaveMethod;

	/** If enabled, you can explicitly set a window size instead of auto determining. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program", meta = (InlineEditConditionToggle))
	uint8 bOverrideWindowSize : 1;

	/** Forcefully set this window size instead of auto determining. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program", meta = (EditCondition = "bOverrideWindowSize"))
	FVector2D OverrideWindowSize;

	/** If enabled, add icon of this program to desktop view where you can double click to open. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	uint8 bAddToDesktop : 1;

	/** If true, then call CreateOsNotification after installation. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Program")
	uint8 bCreateInstallationNotification : 1;

	/** If true, user can only run one instance of this program. If false, user can create as many instances as they want. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS Base Program", meta = (AllowPrivateAccess = "true"))
	uint8 bSingleInstanceOnly : 1;

	/** Some programs might need access to its settings class before they are started. 
	* For example, to determine if the program itself should start automatically when os is started. In this case, you need to enable this.
	* @See: K2_OnSettingsLoad. In Blueprints: On Load Settings
	*/
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCreateSettingsOnProgramCreation : 1;

	/** If true, then this program can run on portable device. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanRunOnPortableDevice : 1;

	/** If true, then C++ can call K2_OnCreate function. Implement OnCreate event in Blueprint Graph */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnCreate : 1;

	/** If true, then C++ can call K2_OnInstall function. Implement OnInstall event in Blueprint Graph */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnInstall : 1;

	/** If true, then C++ can call K2_OnStart function. Implement OnStart event in Blueprint Graph. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnStart : 1;

	/** If true, then C++ can call K2_OnClose function. Implement OnClose event in Blueprint Graph. */
	UPROPERTY(EditDefaultsOnly, AdvancedDisplay, Category = "Yeti OS Base Program")
	uint8 bCanCallOnClose : 1;

	/** True if this program was installed as part of operating system installation. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsSystemInstalledProgram : 1;

	/** Pointer to the actual widget that was created. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	UYetiOS_AppWidget* ProgramWidget;

	/** If true, then this program can be displayed on store. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Store")
	uint8 bSupportStore : 1;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Store", meta = (EditCondition = "bSupportStore"))
	FYetiOsStoreDetail StoreDetail;

	/** The file that is currently opened in this program. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_FileBase* CurrentFileOpened;

	/** Current visibility state. Normal or minimized. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EYetiOsProgramVisibilityState CurrentVisibilityState;

	/** Operating system that owns this program. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_Core* OwningOS;

	/** Points to the settings (if any) for this program. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_ProgramSettings* ProgramSettings;

	/** Process ID assigned to this program while running. Will be -1 (or INDEX_NONE in C++) if not running. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 ProcessID;

	/** Widget that shows the icon of this program. */
	UPROPERTY()
	UYetiOS_AppIconWidget* ProgramIconWidget;

	/** The instance of UYetiOS_DraggablwWindowWidget that owns this program. */
	UPROPERTY()
	class UYetiOS_DraggableWindowWidget* OwningWindow;
	
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

	template <class T>
	static T* CreateProgram(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_BaseProgram> ProgramClass, FYetiOsError& OutErrorMessage, const bool bInstalledWithOS = false)
	{
		static_assert(TIsDerivedFrom<T, UYetiOS_BaseProgram>::IsDerived, "CreateProgram can only cast to classes derived from UYetiOS_BaseProgram.");
		return Cast<T>(CreateProgram(InOS, ProgramClass, OutErrorMessage, bInstalledWithOS));
	}
	
private:

	/**
	* private static UYetiOS_BaseProgram::Internal_StartProgram
	* Starts the program. If single instance, this will return the current instance instead of starting new.
	* @param Program [UYetiOS_BaseProgram*] Program to start.
	* @param OutErrorMessage [FYetiOsError&] Errors if any.
	* @return [UYetiOS_BaseProgram*] Reference to the newly started program.
	**/
	static UYetiOS_BaseProgram* Internal_StartProgram(UYetiOS_BaseProgram* Program, FYetiOsError& OutErrorMessage);

public:

	/**
	* virtual public UYetiOS_BaseProgram::ProgramInstalled
	* Function that calls K2_OnInstall.
	* @See UYetiOS_Core::InstallProgram.
	**/
	virtual void ProgramInstalled();

	/** @See StartProgram(UYetiOS_BaseProgram*& OutProgram, FYetiOsError& OutErrorMessage) */
	virtual const bool StartProgram();

	/** @See StartProgram(UYetiOS_BaseProgram*& OutProgram, FYetiOsError& OutErrorMessage) */
	virtual const bool StartProgram(FYetiOsError& OutErrorMessage);

	/**
	* virtual public UYetiOS_BaseProgram::StartProgram
	* Start this program and add to running list in Operating System.
	* @param OutProgram [UYetiOS_BaseProgram*&] Outputs a reference to the newly started program.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const bool] True if the program was started successfully.
	**/
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
	* virtual public UYetiOS_BaseProgram::OpenFile
	* Opens the given file.
	* @param InFileToOpen [class UYetiOS_FileBase*] File to open.
	* @param bInForceOpenInSameInstance [const bool] Forcefully open in same instance even if Single Instance is turned off.
	* @return [bool] True if given file was opened.
	**/
	virtual bool OpenFile(class UYetiOS_FileBase* InFileToOpen, const bool bInForceOpenInSameInstance);

	/**
	* virtual public UYetiOS_BaseProgram::CloseProgram
	* Close this program.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Program")
	virtual void CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown = false);

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

	/**
	* public UYetiOS_BaseProgram::GetProcessID const
	* Returns the process ID of this program. Only valid if this program is running. If not running this should return INDEX_NONE (-1).
	* @return [int32] Returns process ID.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")	
	inline int32 GetProcessID() const { return ProcessID; }

	/**
	* public UYetiOS_BaseProgram::IsRunning const
	* Checks if this program is running.
	* @return [bool] True if this program is currently running.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Program")	
	bool IsRunning() const { return ProcessID != INDEX_NONE; }

	/**
	* public UYetiOS_BaseProgram::GetProgramIconWidget const
	* Returns Program Icon Widget.
	* @return [class UYetiOS_AppIconWidget*] Program Icon Widget.
	**/
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

	/**
	* private UYetiOS_BaseProgram::Internal_LoadProgramSettings
	* Load program settings. This will load the settings or create a new settings class if necessary.
	* @See CreateProgram, Internal_StartProgram
	* @return [bool] True if program settings was loaded or created.
	**/
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

	/**
	* public UYetiOS_BaseProgram::IsCompatibleWithOS const
	* True if this program can run or be created in target os.
	* @param InOS [class UYetiOS_Core*] Os to check against.
	* @return [bool] True if this program is compatible.
	**/
	bool IsCompatibleWithOS(class UYetiOS_Core* InOS) const;

	/**
	* public UYetiOS_BaseProgram::SetOverrideWindowSize
	* Overrides the window size.
	* @param NewWindowSize [const FVector2D&] New window size.
	**/
	FORCEINLINE void SetOverrideWindowSize(const FVector2D& NewWindowSize)
	{
		bOverrideWindowSize = true;
		OverrideWindowSize = NewWindowSize;
	}

	FORCEINLINE UYetiOS_ProgramSettings* GetProgramSettings() const { return ProgramSettings; }
	FORCEINLINE UObject* GetProgramIcon() const { return ProgramIcon.LoadSynchronous(); }
	FORCEINLINE const bool IsSingleInstanceProgram() const { return bSingleInstanceOnly; }
	FORCEINLINE const bool CanAddToDesktop() const { return bAddToDesktop; }
	FORCEINLINE const float GetProgramSpace() const { return ProgramSpaceInMB; }
	FORCEINLINE TSubclassOf<UYetiOS_AppIconWidget> GetProgramIconWidgetClass() const { return ProgramIconWidgetClass; }
	FORCEINLINE TSubclassOf<UYetiOS_AppWidget> GetProgramWidgetClass() const { return ProgramWidgetClass; }
	FORCEINLINE const FText GetProgramName() const { return ProgramName; }
	FORCEINLINE const FName GetProgramIdentifierName() const { return ProgramIdentifier; }
	FORCEINLINE const bool IsSystemInstalledProgram() const { return bIsSystemInstalledProgram; }
	FORCEINLINE const bool CanSaveSettings() const { return ProgramSettings != nullptr && SaveMethod != EProgramSaveMethod::DoNotSave; }
	FORCEINLINE const bool CanShowPostInstallNotification() const { return bCreateInstallationNotification; }
	FORCEINLINE const bool RequireMinimumOsVersion() const { return bRequireMinimumOperatingSystemVersion; }
	FORCEINLINE const bool SupportsStore() const { return bSupportStore; }
	FORCEINLINE const EYetiOsProgramVisibilityState GetCurrentVisibilityState() const { return CurrentVisibilityState; }
	FORCEINLINE class UYetiOS_DraggableWindowWidget* GetOwningWindow() const { return OwningWindow; }
	FORCEINLINE FYetiOsStoreDetail GetStoreDetail() const { return StoreDetail; }
	FORCEINLINE FYetiOS_Version GetMinimumOsVersionRequired() const { return MinimumOperatingSystemVersionRequired; }
};
