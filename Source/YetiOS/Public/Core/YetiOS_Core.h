// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "YetiOS_Core.generated.h"

class UYetiOS_StartMenu;
class UYetiOS_AppIconWidget;
USTRUCT()
struct FYetiOsNotificationSettings
{
	GENERATED_USTRUCT_BODY();
	
	/** Enable notifications in Operating System. */
	UPROPERTY(EditAnywhere, Category = "Notification Settings")
	uint8 bEnableNotifications : 1;
	
	/** Should we play a sound when a notification is received? */
	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications"))
	uint8 bPlayNotificationSound : 1;

	/** Default sound to play. */
	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications && bPlayNotificationSound"))
	class USoundBase* NotificationSoundDefault;

	/** Warning sound to play if notification is a warning. */
	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications && bPlayNotificationSound"))
	class USoundBase* NotificationSoundWarning;

	/** Error sound to play if notification is an error. */
	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications && bPlayNotificationSound"))
	class USoundBase* NotificationSoundError;

	FYetiOsNotificationSettings()
	{
		bEnableNotifications = true;
		bPlayNotificationSound = true;
		NotificationSoundDefault = nullptr;
		NotificationSoundWarning = nullptr;
		NotificationSoundError = nullptr;
	}
};

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FOnInstallProgramFinishedDelegate, class UYetiOS_BaseProgram*, _Program, const FYetiOsError&, _ErrorMessage, UYetiOS_AppIconWidget*, _IconWidget);

DECLARE_MULTICAST_DELEGATE_OneParam(FOnProgramInstalled, class UYetiOS_BaseProgram*)
DECLARE_MULTICAST_DELEGATE_OneParam(FOnPeekPreview, const bool)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnToggleFileLockForUser, const bool, bIsLocked, class UYetiOS_FileBase*, _File, const FYetiOsUser&, _User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnToggleDirectoryLockForUser, const bool, bIsLocked, class UYetiOS_DirectoryBase*, _Dir, const FYetiOsUser&, _User);

/*************************************************************************
* File Information:
YetiOS_Core.h

* Description:
The main Operating System class.
*************************************************************************/
UCLASS(hidedropdown, Blueprintable, DisplayName = "Operating System")
class YETIOS_API UYetiOS_Core : public UObject
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseDevice;
	
#if WITH_EDITOR
	friend class UYetiOS_ThumbnailRenderer;
#endif
	
	FTimerHandle TimerHandle_OsInstallation;

	FDelegateHandle DelegateHandle_Lock;
	FDelegateHandle DelegateHandle_Unlock;
	
private:

	/** Operating system name. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	FText OsName;

	/** Version of this operating system. Eg: 0.1 or 1.2 or 1.2.3 */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	FYetiOS_Version OsVersion;

	/** Logo of this Operating System. Ex: Tux (mascot) penguin for Linux. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* OsIcon;

	/** How much space (in mega bytes) does this OS need to install on HDD. Default 10000 means 10 GB. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (UIMin = "1000", ClampMin = "1000", UIMax = "20000"))
	float InstallationSpaceInMB;

	/** A collection of programs which user can install from a "repo" simulating the effect of "sudo apt-get install program-identifier". */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	class UYetiOS_ProgramsRepository* ProgramsRepository;

	/** List of devices this operating system is compatible with. If you try to load this OS on incompatible device it will result in Blue Screen. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TArray<TSubclassOf<class UYetiOS_BaseDevice>> CompatibleDevices;

	/** Release state of this OS. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	EYetiOsOperatingSystemReleaseState ReleaseState;

	/** The main OS widget class. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TSubclassOf<class UYetiOS_OsWidget> OsWidgetClass;

	/** Reference to the settings object. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	class UYetiOS_SystemSettings* SystemSettings;

	/** Class that defines system taskbar. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TSubclassOf<class UYetiOS_Taskbar> TaskbarClass;

	/** Settings for notification. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	FYetiOsNotificationSettings NotificationSettings;

	/** A root user for this OS. Defaults to root. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", AdvancedDisplay)
	FYetiOsUser RootUser;

	/** Root command name. Defaults to sudo. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", AdvancedDisplay)
	FText RootCommand;

	/** List of users for this OS. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", AdvancedDisplay)
	TArray<FYetiOsUser> OsUsers;

	/** A default template directory with no name. This template directory is used to create new directories. Must not be null. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TSubclassOf<UYetiOS_DirectoryBase> TemplateDirectory;

	/** Minimum time to install. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (UIMin = "5", ClampMin = "1", UIMax = "60", ClampMax = "120"))
	float MinInstallationTime;

	/** Maximum time to install. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (UIMin = "10", ClampMin = "1", UIMax = "100", ClampMax = "120"))	
	float MaxInstallationTime;

	/** Auto calculated time to install based on different factors. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, AdvancedDisplay)
	float CalculatedInstallationTime;

	/** The device that this OS is running on. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BaseDevice* Device;

	/** Reference to the OS widget. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_OsWidget* OsWidget;

	/** List of actively running programs. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TMap<int32, class UYetiOS_BaseProgram*> RunningPrograms;

	/** List of installed programs. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<class UYetiOS_BaseProgram*> InstalledPrograms;

	/** The main root directory. Cannot be null. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	mutable class UYetiOS_DirectoryRoot* RootDirectory;

	/** The user that is currently active. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOsUser CurrentActiveUser;

	/** List of all created directories. Used for save game information. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<const UYetiOS_DirectoryBase*> AllCreatedDirectories;

	/** Taskbar that was created if Taskbar Class was not null. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_Taskbar* Taskbar;

	/** True if this Operating System was pre-installed with the device. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsPreInstalled : 1;

	/** The world that this Operating System belongs to. */
	UPROPERTY()
	class UWorld* OsWorld;

	/** List of current dialog widgets. Can include modal dialogs as well. */
	UPROPERTY()
	TArray<class UYetiOS_DialogWidget*> CurrentDialogWidgets;

	/** Main notification manager. */
	class FYetiOsNotificationManager* NotificationManager;

	/** Z Order of lastly opened windows. */
	int32 CurrentZOrder;

	/** Weak pointer to the desktop directory. */
	TWeakObjectPtr<UYetiOS_DirectoryBase> DesktopDirectory;

public:

	/** Delegate called when program is installed. @See InstallProgram */
	FOnProgramInstalled OnProgramInstalled;

	/** Delegate called when peek desktop is activated. @See UYetiOS_Taskbar::PeekDesktop */
	FOnPeekPreview OnPeekPreview;

	UPROPERTY(BlueprintAssignable, Category = "Yeti OS|Delegates")
	FOnToggleFileLockForUser OnToggleFileLockForUser;

	UPROPERTY(BlueprintAssignable, Category = "Yeti OS|Delegates")
	FOnToggleDirectoryLockForUser OnToggleDirectoryLockForUser;

	UYetiOS_Core();

	static const FString PATH_DELIMITER;

	UFUNCTION(BlueprintPure, Category = "Yeti Global", meta = (DisplayName = "Equal (Yeti OS User)", CompactNodeTitle = "Same User", Keywords = "== equal same"))
	static bool EqualEqual_YetiOsUser(const FYetiOsUser& A, const FYetiOsUser& B)
	{
		return A == B;
	}

	UFUNCTION(BlueprintPure, Category = "Yeti Global", meta = (DisplayName = "Compatible Version", CompactNodeTitle = "Compatible", Keywords = "== equal same compatible check"))
	static bool IsVersionsCompatible(const FYetiOS_Version& VersionA, const FYetiOS_Version& VersionB)
	{
		return VersionA < VersionB;
	}

	/**
	* public static UYetiOS_Core::CreateOperatingSystem
	* Creates an operating system for the given device and returns it. 
	* Will be nullptr if the given device is not compatible.
	* @param InParentDevice [class UYetiOS_BaseDevice*] Device this operating system belongs to. If this device is not compatible this function will return nullptr.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [UYetiOS_Core*] Returns newly created operating system.
	**/
	static UYetiOS_Core* CreateOperatingSystem(class UYetiOS_BaseDevice* InParentDevice, FYetiOsError& OutErrorMessage);

	/**
	* public static UYetiOS_Core::CloseDialogWidget
	* Closes the given dialog widget and removes it from CurrentDialogWidgets set.
	* @param InOS [UYetiOS_Core*] OS to remove the dialog widget from.
	* @param InDialogWidget [UYetiOS_DialogWidget*] Dialog widget to remove.
	* @return [const bool] True if the dialog was removed.
	**/
	static const bool CloseDialogWidget(UYetiOS_Core* InOS, UYetiOS_DialogWidget* InDialogWidget);

	/**
	* public static UYetiOS_Core::OpenDialogWidget
	* Opens a dialog box with title and message. You can use this to simulate the effect of Messagebox, OpenFileDialog etc.
	* @param InOS [UYetiOS_Core*] Operating System reference.
	* @param InDialogWidgetClass [TSubclassOf<class UYetiOS_DialogWidget>] Dialog widget class.
	* @param DialogClass [TSubclassOf<class UYetiOS_BaseProgram>] Dialog program to open.
	* @param InMessage [const FText&] Message to display.
	* @param InTitle [FText] Title to display.
	* @param OverrideWindowSize [const FVector2D&] Overrides the size of window. If -1, it will use auto size. If 0, automatically calculates the size relative to viewport. If > 0, use it as size.
	* @param bIsModalDialog [const bool] Should this dialog be a modal dialog?
	* @return [class UYetiOS_DialogWidget*] Reference to the newly created dialog widget.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Global")
	static class UYetiOS_DialogWidget* OpenDialogWidget(UYetiOS_Core* InOS, TSubclassOf<class UYetiOS_DialogWidget> InDialogWidgetClass, TSubclassOf<class UYetiOS_BaseDialogProgram> DialogClass, const FText& InMessage, FText InTitle = INVTEXT("Dialog"), const FVector2D& OverrideWindowSize = FVector2D::ZeroVector, const bool bIsModalDialog = true, EYetiOS_DialogType InDialogType = EYetiOS_DialogType::Ok);

	/**
	* public static UYetiOS_Core::GetVersionString
	* Returns a string representation of given version.
	* @param InVersion [const struct FYetiOS_Version&] Version to get string from.
	* @param bIgnorePatch [const bool] Ignore patch. Example: If true and version is 1.2.1, then return 1.2
	* @return [FString] String representation of verison.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")	
	static FString GetVersionString(const struct FYetiOS_Version& InVersion, const bool bIgnorePatch = false)
	{
		return InVersion.ToString(bIgnorePatch);
	}

	/**
	* public static UYetiOS_Core::EvaluateMathExpression
	* Evaluates the given expression. If expression failed, this will return false. Otherwise OutValue contains the result of the expression.
	* @param InExpression [const FString&] Expression to evaluate.
	* @param OutValue [float&] Result of the expression if return is true.
	* @return [bool] True if successful, false if equation failed.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti Global")	
	static bool EvaluateMathExpression(const FString& InExpression, float& OutValue);

	/**
	* public static UYetiOS_Core::GetSystemDirectories
	* Returns an array of directories that are specified as system.
	* @param InOS [class UYetiOS_Core*] Operating system to get from.
	* @return [TArray<class UYetiOS_DirectoryBase*>] Array of directories.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")	
	static TArray<class UYetiOS_DirectoryBase*> GetSystemDirectories(class UYetiOS_Core* InOS);

	/**
	* public static UYetiOS_Core::GetTimeAsText
	* Converts a passed in date & time to a text, formatted as a time using an invariant timezone. This will use the given date & time as-is, so it's assumed to already be in the correct timezone.
	* Returns time in short style. Example: 1:00 PM
	* @param InDateTime [const FDateTime&] Time to convert.
	* @param TimeFormat [EYetiOSTimeFormat] Formats the time. Example: Hour:Minutes, Hour:Minutes:Seconds, Hour:Minutes:Seconds <TimeZone>
	* @return [const FText] Time as text. Example: 1:00 PM
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")
	static const FText GetTimeAsText(const FDateTime& InDateTime, EYetiOSTimeFormat TimeFormat = EYetiOSTimeFormat::Medium);

	/**
	* public static UYetiOS_Core::GetColorCollectionOfTheme
	* Helper function to retrieve color collection of given theme.
	* @param InOS [class UYetiOS_Core*] OS to get settings from.
	* @param InTheme [EYetiOsThemeMode] Color collection of theme to get.
	* @param OutCollection [FYetiOsColorCollection&] Outputs a valid collection if return value is true.
	* @return [bool] True if a collection was found for the given theme.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global|Themes")
	static bool GetColorCollectionOfTheme(class UYetiOS_Core* InOS, EYetiOsThemeMode InTheme, FYetiOsColorCollection& OutCollection);

	/**
	* public static UYetiOS_Core::GetColorFromCurrent
	* Returns color from current color collection that is set.
	* @param InOS [class UYetiOS_Core*] OS to get settings from.
	* @param InColorType [EYetiOsColorTypes] Type of color to get.
	* @return [FLinearColor] Color of given type.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global|Themes")	
	static FLinearColor GetColorFromCurrent(class UYetiOS_Core* InOS, EYetiOsColorTypes InColorType);

	/**
	* public UYetiOS_Core::ListenForLock
	* This function will be called whenever a file or directory is locked.
	* @See ListenForUnlock()
	* @param Object [class UObject*] This can be a file or directory.
	* @param InUser [const FYetiOsUser&] User that was locked.
	**/
	void ListenForLock(class UObject* Object, const FYetiOsUser& InUser);

	/**
	* public UYetiOS_Core::ListenForUnlock
	* This function will be called whenever a file or directory is unlocked.
	* @See ListenForLock()
	* @param Object [class UObject*] This can be a file or directory.
	* @param InUser [const FYetiOsUser&] User that was unlocked.
	**/
	void ListenForUnlock(class UObject* Object, const FYetiOsUser& InUser);

	/**
	* public UYetiOS_Core::CreateOsNotification
	* Creates an OS notification and add it to the notification manager.
	* @param InNewNotification [const FYetiOsNotification] Notification to add.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")	
	void CreateOsNotification(const FYetiOsNotification InNewNotification);

	/**
	* public UYetiOS_Core::ToggleLockOnFile
	* Toggles lock state on target file.
	* @param TargetFile [class UYetiOS_FileBase*] File to lock or unlock.
	* @param bLock [bool] True to lock. False to unlock.
	* @param InUser [const FYetiOsUser&] User that should be locked or unlocked.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")	
	void ToggleLockOnFile(class UYetiOS_FileBase* TargetFile, bool bLock, const FYetiOsUser& InUser);

	/**
	* public UYetiOS_Core::ToggleLockOnDirectory
	* Toggles lock state on target directory.
	* @param TargetDirectory [class UYetiOS_DirectoryBase*] Directory to be locked.
	* @param bLock [bool] True to lock. False to unlock.
	* @param InUser [const FYetiOsUser&] User that should be locked or unlocked.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")	
	void ToggleLockOnDirectory(class UYetiOS_DirectoryBase* TargetDirectory, bool bLock, const FYetiOsUser& InUser);

	/**
	* public UYetiOS_Core::StartOperatingSystem
	* Starts operating system. If not installed, this will start the installation.
	* @param bIsInstalled [const bool] Is the os installed?
	* @param bShowBsodIfInstallationFails [const bool] If true, then show blue screen widget if installation fails. Recommended to keep this true.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const bool] Returns true if OS was started successfully or installation was started successfully.
	**/
	const bool StartOperatingSystem(const bool bIsInstalled, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_Core::ShutdownOS
	* Shutdown the operating system. Closes all running programs and begin shutdown in OsWidget.
	**/
	void ShutdownOS();

	/**
	* public UYetiOS_Core::RestartOS
	* Restarts the operating system. Closes all running programs and begin restart in OsWidget.
	**/
	void RestartOS();

	/**
	* public UYetiOS_Core::CloseAllPrograms
	* Closes all programs
	* @param bIsOperatingSystemShuttingDown [const bool] True if this OS is shutting down.
	**/
	void CloseAllPrograms(const bool bIsOperatingSystemShuttingDown);

	/**
	* public UYetiOS_Core::AddNewUser
	* Add a new user to the operating system.
	* @WARNING You cannot add root user.
	* @param InNewUser [FYetiOsUser] New user to add to list of OsUsers.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [bool] True if user was successfully added.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")
	bool AddNewUser(FYetiOsUser InNewUser, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_Core::InstallProgram
	* Installs the given program class to this OS. If the given program is already installed this will return null.
	* @param InProgramToInstall [TSubclassOf<UYetiOS_BaseProgram>] Program to install.
	* @param OutErrorMessage [FYetiOsError&] Outputs any error message. If the return value is nullptr then check this error message.
	* @param OutIconWidget [UYetiOS_AppIconWidget*&] Outputs the icon widget that can be added to desktop. Only valid if the return value is != nullptr.
	* @return [UYetiOS_BaseProgram*] Returns an instance of the newly installed program.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")	
	UYetiOS_BaseProgram* InstallProgram(TSubclassOf<UYetiOS_BaseProgram> InProgramToInstall, FYetiOsError& OutErrorMessage, UYetiOS_AppIconWidget*& OutIconWidget);

	/**
	* public UYetiOS_Core::InstallProgramFromPackage
	* Install a given program (or package) from the repository. A valid repository library should be defined and the program you are looking must have a valid identifier.
	* @param InProgramIdentifier [const FString&] Identifier of the program to look for. This is NOT the name of the program. @See UYetiOS_BaseProgram::ProgramIdentifier
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @param OutIconWidget [UYetiOS_AppIconWidget*&] A reference to the icon widget if program was successfully found.
	* @return [UYetiOS_BaseProgram*] Reference to the program that was installed.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")
	UYetiOS_BaseProgram* InstallProgramFromPackage(const FString& InProgramIdentifier, FYetiOsError& OutErrorMessage, UYetiOS_AppIconWidget*& OutIconWidget);

	/**
	* public UYetiOS_Core::InstallProgramFromPackageWithTimer
	* Installs the program after the time expires. If time is 0, instantly install the program.
	* @param InProgramIdentifier [const FString&] Program identifier to install.
	* @param Time [float] Time (in seconds) taken to install the program.
	* @param Callback [const FOnInstallProgramFinishedDelegate&] Callback to execute when timer expires.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")
	void InstallProgramFromPackageWithTimer(const FString& InProgramIdentifier, float Time, const FOnInstallProgramFinishedDelegate& Callback);

	/**
	* public UYetiOS_Core::StartProgram
	* Checks if any program with given identifier is found. If so, start it.
	* @param ProgramIdentifier [const FName&] Identifier of the program.
	* @param OutErrorMessage [FYetiOsError&] Any error message.
	* @return [bool] True if program was started. False otherwise.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")	
	bool StartProgram(const FName& ProgramIdentifier, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_Core::IsProgramRunning const
	* Checks if the given program is running.
	* @param InProgram [const class UYetiOS_BaseProgram*] Program to check.
	* @return [const bool] True if program is in running list.
	**/
	const bool IsProgramRunning(const class UYetiOS_BaseProgram* InProgram) const;

	/**
	* public UYetiOS_Core::IsProgramRunning const
	* Checks if a program with given identifier is running.
	* @param InProgramIdentifier [const FName&] Identifier to check for.
	* @param OutProgram [class UYetiOS_BaseProgram*&] Program that is running.
	* @return [const bool] True if program is in running list.
	**/
	const bool IsProgramRunning(const FName& InProgramIdentifier, class UYetiOS_BaseProgram*& OutProgram) const;

	/**
	* public UYetiOS_Core::IsProcessIdValid
	* Checks if the given process id is valid.
	* @param InProcessID [const int32&] Process id to check for.
	* @param OutProgram [class UYetiOS_BaseProgram*&] Program associated with the process id.
	* @return [const bool] True if a program is running in given process id.
	**/
	const bool IsProcessIdValid(const int32& InProcessID, class UYetiOS_BaseProgram*& OutProgram);

	/**
	* virtual public UYetiOS_Core::AddRunningProgram
	* Adds the given program to running programs and returns the process ID (index) of the newly added program.
	* Returns INDEX_NONE (-1) if program could not be added.
	* @param InNewProgram [const class UYetiOS_BaseProgram*] Program to add.
	* @param OutErrorMessage [FYetiOsError&] Outputs any error message (if any).
	* @return [int32] Consider this as the process ID.
	**/
	virtual int32 AddRunningProgram(const class UYetiOS_BaseProgram* InNewProgram, FYetiOsError& OutErrorMessage);

	/**
	* virtual public UYetiOS_Core::CloseRunningProgram
	* Closes the given program and removes it from running list.
	* @param InProgram [class UYetiOS_BaseProgram*] Program to close.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	**/
	virtual void CloseRunningProgram(class UYetiOS_BaseProgram* InProgram, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_Core::SetActiveUser
	* Sets the given user to active user.
	* @param InNewUser [FYetiOsUser] User to be active.
	**/
	void SetActiveUser(FYetiOsUser InNewUser);

	/**
	* public UYetiOS_Core::ChangePassword
	* Sets a new password. Returns true only if the password is different from previous one.
	* @param InNewUser [FYetiOsUser] User to change password.
	* @param InNewPassword [FText] New password to change to.
	* @return [bool] True if password was changed.
	**/
	bool ChangePassword(UPARAM(ref) FYetiOsUser& InNewUser, FText InNewPassword);

	/**
	* public UYetiOS_Core::DestroyOS
	* Destroys the Operating System.
	* @See: UYetiOS_BaseDevice::DestroyYetiDevice();
	**/
	void DestroyOS();

	/**
	* public UYetiOS_Core::UpdateWindowZOrder
	* Updates the Z Order of the given window, bringing to front. 
	* If any modal dialog is open this will return false.
	* @param InWindow [class UYetiOS_DraggableWindowWidget*] Window to update.
	* @return [const bool] True if z order was updated.
	**/
	const bool UpdateWindowZOrder(class UYetiOS_DraggableWindowWidget* InWindow);

	/**
	* public UYetiOS_Core::GetOsVersion const
	* Returns the operating system version.
	* @return [const FYetiOS_Version] Os version.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	FYetiOS_Version GetOsVersion() const { return OsVersion; }

	/**
	* public UYetiOS_Core::GetOsIcon const
	* Returns the UObject icon.
	* @return [UObject*] Icon.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	UObject* GetOsIcon() const { return OsIcon; }

	/**
	* public UYetiOS_Core::GetAllUsers const
	* Returns all users for this OS.
	* @return [const TArray<FYetiOsUser>] An array of users.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	const TArray<FYetiOsUser> GetAllUsers() const { return OsUsers; }
	
protected:

	/**
	* protected UYetiOS_Core::HasValidRootDirectoryClass const
	* True if this OS has a valid root directory class.
	* @return [const bool] True if valid root directory class is available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const bool HasValidRootDirectoryClass() const;
		
	/**
	* protected UYetiOS_Core::GetMainCpu const
	* Returns the primary CPU.
	* @return [UYetiOS_CPU*] CPU Object.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	UYetiOS_CPU* GetMainCpu() const;

private:

	/**
	* private UYetiOS_Core::Internal_FinishOperatingSystemInstallation
	* Automatically gets called when installation timer finishes.	
	**/
	void Internal_FinishOperatingSystemInstallation();

	/**
	* private UYetiOS_Core::Internal_FindProgramFromPackage
	* Tries to find the given package identifier from repository library
	* @param InProgramIdentifier [const FName&] Identifier of the program. This is NOT the program name. @See UYetiOS_BaseProgram::ProgramIdentifier
	* @return [TSubclassOf<class UYetiOS_BaseProgram>] Program class. Can be null if program was not found.
	**/
	TSubclassOf<class UYetiOS_BaseProgram> Internal_FindProgramFromPackage(const FName& InProgramIdentifier);

	/**
	* private UYetiOS_Core::Internal_ConsumeSpace
	* Reduces the given space from Hard Disk.
	* @param InSpaceToConsume [float] Size to reduce.
	* @return [const bool] True if space was actually reduced.
	**/
	const bool Internal_ConsumeSpace(float InSpaceToConsume);

	/**
	* private UYetiOS_Core::Internal_InstallStartupPrograms
	* Installs all programs defined in ProgramsToInstall.
	**/
	void Internal_InstallStartupPrograms();

public:

	/**
	* public UYetiOS_Core::OnOperatingSystemLoadedFromSaveGame
	* Loads the operating system from a save state. 
	* @See UYetiOS_BaseDevice::StartDevice
	* @param LoadGameInstance [const class UYetiOS_SaveGame*&] Load game instance. @See UYetiOS_BaseDevice::StartDevice
	* @param OutErrorMessage [FYetiOsError&] Any error message.
	**/
	void OnOperatingSystemLoadedFromSaveGame(const class UYetiOS_SaveGame*& LoadGameInstance, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_Core::NotifyBatteryLevelChange
	* Notifies the operating system that this device consumed or charged battery.
	* @param CurrentBatteryLevel [const float&] Remaining battery level.
	**/
	void NotifyBatteryLevelChange(const float& CurrentBatteryLevel);

	/**
	* public UYetiOS_Core::NotifyLowBattery
	* Notifies the operating system that this device is running on low battery.
	* @See UYetiOS_PortableDevice::Internal_ConsumeBattery()
	**/
	void NotifyLowBattery(const bool bIsLowBattery);

	/**
	* public UYetiOS_Core::IsProgramInstalled const
	* Checks if the given program (by identifier) is installed in this Operating System.
	* @param InProgramIdentifier [const FName&] Program identifier reference.
	* @param OutFoundProgram [UYetiOS_BaseProgram*&] If the return is true, then this will point to the installed program.
	* @param OutErrorMessage [FYetiOsError&] Outputs any error message.
	* @return [const bool] True if the given program is installed.
	**/
	const bool IsProgramInstalled(const FName& InProgramIdentifier, UYetiOS_BaseProgram*& OutFoundProgram, FYetiOsError& OutErrorMessage) const;

	/**
	* public UYetiOS_Core::IsProgramInstalled const
	* Checks if the given program (by identifier) is installed in this Operating System.
	* @param InProgramIdentifier [const FName&] Program identifier reference.
	* @return [bool] True if the given program is installed.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	bool IsProgramInstalled(const FName& InProgramIdentifier) const;
	
	/**
	* public UYetiOS_Core::AddToCreatedDirectories
	* Adds the given directory to AllCreatedDirectories array.
	* @param InDirectory [const UYetiOS_DirectoryBase*] Directory to add.
	**/
	void AddToCreatedDirectories(const UYetiOS_DirectoryBase* InDirectory);

	/**
	* public UYetiOS_Core::HasRepositoryLibrary const
	* Checks if the repository library is valid and has classes added to it.
	* @return [bool] True if valid library is found.
	**/
	inline bool HasRepositoryLibrary() const;

	/**
	* public UYetiOS_Core::GetOwningDevice const
	* Returns the device owned by this operating system.
	* @return [UYetiOS_BaseDevice*] Owning device.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	inline UYetiOS_BaseDevice* GetOwningDevice() const { return Device; }

	/**
	* public UYetiOS_Core::GetRootUser const
	* Returns the root user.
	* @return [const FYetiOsUser] Root user.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	const FYetiOsUser GetRootUser() const { return RootUser; }

	/**
	* public UYetiOS_Core::GetCurrentUser const
	* Returns the current active user.
	* @return [const FYetiOsUser] Active user.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	const FYetiOsUser GetCurrentUser() const { return CurrentActiveUser; }
	
	/**
	* public UYetiOS_Core::GetRootDirectory const
	* Returns the root directory. Will create if null and valid class is available.
	* @return [UYetiOS_DirectoryRoot*] Root directory reference.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	UYetiOS_DirectoryRoot* GetRootDirectory();

	/**
	* public UYetiOS_Core::DirectoryExists const
	* Checks if the given path exists or no. Example: /home/desktop
	* @param InDirectoryPath [const FString&] Path to check. Example: /home/desktop
	* @return [const bool] True if the path is valid.
	**/
	const bool DirectoryExists(const FString& InDirectoryPath) const;
	
	/**
	* public UYetiOS_Core::DirectoryExists const
	* Checks if the given directory path exists. Example: /Home/Desktop
	* @param InDirectoryPath [const FString&] Path to test.
	* @param OutFoundDirectory [UYetiOS_DirectoryBase*&] Reference to the directory that was found.
	* @return [const bool] True if the given path is valid.
	**/
	const bool DirectoryExists(const FString& InDirectoryPath, UYetiOS_DirectoryBase*& OutFoundDirectory) const;

	/**
	* public UYetiOS_Core::CreateDirectoryInPath
	* Creates a directory in path recursively. For example if you pass /home/desktop/mydirectory/anotherdir then it will check if each directory exists and creates if its not found.
	* @param InDirectoryPath [const FString&] Directory path to create.
	* @param bHidden [const bool] Create directories as hidden.
	* @param OutErrorMessage [FYetiOsError&] Outputs any error message (if any).
	* @param InDirName [const FText&] Directory name to create.
	* @return [UYetiOS_DirectoryBase*] Created directory in path.
	**/
	UYetiOS_DirectoryBase* CreateDirectoryInPath(const FString& InDirectoryPath, const bool bHidden, FYetiOsError& OutErrorMessage, const FText& InDirName = FText::GetEmpty());
		
	/**
	* public UYetiOS_Core::GetOsName const
	* Returns the name of this operating system.
	* @return [const FText] Operating System name.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const FText GetOsName() const { return OsName; }

	/**
	* public UYetiOS_Core::GetTemplateDirectory const
	* Returns the template directory.
	* @return [TSubclassOf<UYetiOS_DirectoryBase>] Template directory class.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	inline TSubclassOf<UYetiOS_DirectoryBase> GetTemplateDirectory() const { return TemplateDirectory; }
		
	/**
	* public UYetiOS_Core::GetReleaseState const
	* Returns the release state of this operating system. 
	* @return [const EYetiOsOperatingSystemReleaseState] Release state of this OS.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const EYetiOsOperatingSystemReleaseState GetReleaseState() const { return ReleaseState; }

	/**
	* public UYetiOS_Core::GetRunningPrograms const
	* Gets all running programs.
	* @return [TArray<class UYetiOS_BaseProgram*>] An array of running programs.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	TArray<class UYetiOS_BaseProgram*> GetRunningPrograms() const;

	/**
	* public UYetiOS_Core::GetRunningProgramByIdentifier const
	* Finds a program that is running by its unique identifier.
	* @param InIdentifier [const FName&] Identifier to look for.
	* @return [class UYetiOS_BaseProgram*] Program that is running or nullptr.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	class UYetiOS_BaseProgram* GetRunningProgramByIdentifier(const FName& InIdentifier) const;

	/**
	* public UYetiOS_Core::GetInstalledPrograms const
	* Returns an array of programs already installed on this device.
	* @param bSystemProgramsOnly [const bool] Enable to return apps installed with Operating System.
	* @return [const TArray<class UYetiOS_BaseProgram*>] Array of programs installed on this device.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	const TArray<class UYetiOS_BaseProgram*> GetInstalledPrograms(const bool bSystemProgramsOnly = false) const;

	/**
	* public UYetiOS_Core::GetSystemSettings const
	* Returns the system settings. Only valid if SystemSettingsClass was set.
	* @return [UYetiOS_SystemSettings*] Returns System Settings reference.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	UYetiOS_SystemSettings* GetSystemSettings() const { return SystemSettings; }

	/**
	* public UYetiOS_Core::GetAllProgramsFromRepositoryLibrary const
	* Outputs an array of programs from the repository package provided if this OS has a valid repository library assigned.
	* @param OutPrograms [TArray<TSubclassOf<class UYetiOS_BaseProgram>>&] Array with all program classes from respoitory library. Valid ONLY if return value is true.
	* @return [const bool] Returns true if a valid library was found.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")
	const bool GetAllProgramsFromRepositoryLibrary(TArray<TSubclassOf<class UYetiOS_BaseProgram>>& OutPrograms);

	/**
	* public UYetiOS_Core::GetTaskbar const
	* Returns true if there is a valid taskbar associated with this operating system.
	* @param OutTaskbar [UYetiOS_Taskbar*&] Gets a reference to the taskbar owned by this operating system.
	* @return [bool] True if there is a taskbar available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	bool GetTaskbar(UYetiOS_Taskbar*& OutTaskbar) const;

	/**
	* public UYetiOS_Core::GetStartMenu const
	* Returns true if there is a valid start menu associated with this operating system.
	* @param OutStartMenu [UYetiOS_StartMenu*&] Gets a reference to the start menu owned by this operating system.
	* @return [bool] True if there is a start menu available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	bool GetStartMenu(UYetiOS_StartMenu*& OutStartMenu) const;

	/**
	* public UYetiOS_Core::GetDesktopDirectory const
	* Returns true if there is a valid Desktop associated with this operating system.
	* @param OutDesktopDir [UYetiOS_DirectoryBase*&] Gets a reference to Desktop directory.
	* @return [bool] True if desktop directory is available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	bool GetDesktopDirectory(UYetiOS_DirectoryBase*& OutDesktopDir) const;

	/**
	* public UYetiOS_Core::IsModalDialogOpen const
	* Returns true if currently opened dialog widget is modal or not. If there is no dialog widget open, this will return false.
	* @return [bool] True if dialog widget is modal.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	bool IsModalDialogOpen() const;

	/**
	* public UYetiOS_Core::GetProgramFromInstalled const
	* Returns the program with given identifier if it is installed. nullptr if not installed.
	* @param InIdentifier [const FName&] Identifier of the program to check.
	* @return [class UYetiOS_BaseProgram*] Pointer to the program that is installed. Null if not installed.
	**/
	class UYetiOS_BaseProgram* GetProgramFromInstalled(const FName& InIdentifier) const;

protected:

	/**
	* protected UYetiOS_Core::K2_OnBatteryLevelChanged
	* Event that is called when battery level changes. Either by depleting or charging.
	* @param bConsumedBattery [const float&] Current battery level.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS", DisplayName = "OnBatteryLevelChanged")
	void K2_OnBatteryLevelChanged(const float& CurrentBatteryLevel);

public:

	void SetOsWidget(class UYetiOS_OsWidget* InWidget)
	{
		OsWidget = InWidget;
	}

	void SetTaskbar(class UYetiOS_Taskbar* InTaskbar)
	{
		Taskbar = InTaskbar;
	}
	
	/** Returns an array of compatible device classes for this operating system. */
	FORCEINLINE const TArray<TSubclassOf<class UYetiOS_BaseDevice>> GetCompatibleDeviceClasses() const { return CompatibleDevices; }

	/** Returns the UMG class for OS widget. */
	FORCEINLINE TSubclassOf<class UYetiOS_OsWidget> GetOsWidgetClass() const { return OsWidgetClass; }

	/** Returns the taskbar class */
	FORCEINLINE TSubclassOf<class UYetiOS_Taskbar> GetTaskbarClass() const { return TaskbarClass; }

	/** Returns a reference to the Operating system widget created using OsWidgetClass. */
	FORCEINLINE UYetiOS_OsWidget* GetOsWidget() const { return OsWidget; }	

	FORCEINLINE const TArray<const UYetiOS_DirectoryBase*> GetAllCreatedDirectories() const { return AllCreatedDirectories; }
	FORCEINLINE const FText GetRootCommand() const { return RootCommand; }
	FORCEINLINE class USoundBase* GetNotificationSound(const FYetiOsNotification& InNotification) const
	{
		USoundBase* MySound = nullptr;
		if (NotificationSettings.bPlayNotificationSound)
		{
			EYetiOsNotificationType NotifyType = InNotification.Level;
			switch (NotifyType)
			{
				case EYetiOsNotificationType::TYPE_Warning:
					MySound = NotificationSettings.NotificationSoundWarning;
					break;
				case EYetiOsNotificationType::TYPE_Error:
					MySound = NotificationSettings.NotificationSoundError;
					break;
				default:
					MySound = NotificationSettings.NotificationSoundDefault;
					break;
			}
		}
		return MySound;
	}
};

class YETIOS_API FYetiOsNotificationManager
{
private:

	TArray<FYetiOsNotification> Notifications;

public:

	FYetiOsNotificationManager() {}

	~FYetiOsNotificationManager()
	{
		Notifications.Empty();
	}

	static FORCEINLINE FYetiOsNotificationManager* CreateNotificationManager()
	{
		return new FYetiOsNotificationManager();
	}

	static FORCEINLINE void Destroy(FYetiOsNotificationManager* InNotificationManager)
	{
		delete InNotificationManager;
	}

	FORCEINLINE void LogNotification(const FYetiOsNotification InNewNotification)
	{
		Notifications.Add(InNewNotification);
	}

	FORCEINLINE const TArray<FYetiOsNotification> GetNotifications() const
	{
		return Notifications;
	}
};
