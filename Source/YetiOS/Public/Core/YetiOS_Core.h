// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "YetiOS_Core.generated.h"

USTRUCT()
struct FYetiOsNotificationSettings
{
	GENERATED_USTRUCT_BODY();
	
	UPROPERTY(EditAnywhere, Category = "Notification Settings")
	uint8 bEnableNotifications : 1;
	
	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications"))
	uint8 bPlayNotificationSound : 1;

	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications"))
	class USoundBase* NotificationSoundDefault;

	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications"))
	class USoundBase* NotificationSoundWarning;

	UPROPERTY(EditAnywhere, Category = "Notification Settings", meta = (EditCondition = "bEnableNotifications"))
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


UCLASS(Abstract, Blueprintable, DisplayName = "Operating System")
class YETIOS_API UYetiOS_Core : public UObject
{
	GENERATED_BODY()
	
	friend class UYetiOS_BaseDevice;
	
	FTimerHandle TimerHandle_OsInstallation;
	
private:

	/* Operating system name. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	FText OsName;

	/* Version of this operating system. Eg: 1.0.57784.1 */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	FText OsVersion;

	/* Logo of this Operating System. Ex: Tux (mascot) penguin for Linux. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* OsIcon;

	/* How much space does this OS need to install on HDD. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (UIMin = "1", ClampMin = "1", UIMax = "10"))
	float InstallationSpace;

	/* A collection of programs which user can install from a "repo" simulating the effect of "sudo apt-get install program-identifier". */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	class UObjectLibrary* RepositoryLibrary;

	/* List of devices this operating system is compatible with. If you try to load this OS on incompatible device it will result in Blue Screen. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TArray<TSubclassOf<class UYetiOS_BaseDevice>> CompatibleDevices;

	/* Release state of this OS. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	EYetiOsOperatingSystemReleaseState ReleaseState;

	/* The main OS widget class. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TSubclassOf<class UYetiOS_OsWidget> OsWidgetClass;

	/** Class that defines system settings. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TSubclassOf<class UYetiOS_SystemSettings> SystemSettingsClass;

	/** Settings for notification. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	FYetiOsNotificationSettings NotificationSettings;

	/* A root user for this OS. Defaults to root. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", AdvancedDisplay)
	FYetiOsUser RootUser;

	/** Root command name. Defaults to sudo. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", AdvancedDisplay)
	FText RootCommand;

	/* List of users for this OS. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", AdvancedDisplay)
	TArray<FYetiOsUser> OsUsers;

	/* List of pre-defined programs to install when you install this OS. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TArray<TSubclassOf<UYetiOS_BaseProgram>> ProgramsToInstall;

	/* A default template directory with no name. This template directory is used to create new directories. Must not be null. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS")
	TSubclassOf<UYetiOS_DirectoryBase> TemplateDirectory;

	/* Minimum time to install. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (UIMin = "5", ClampMin = "1", UIMax = "60", ClampMax = "120"))
	float MinInstallationTime;

	/* Maximum time to install. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS", meta = (UIMin = "10", ClampMin = "1", UIMax = "100", ClampMax = "120"))	
	float MaxInstallationTime;

	/* Auto calculated time to install based on different factors. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug, AdvancedDisplay)
	float CalculatedInstallationTime;

	/* Remaining HDD space (in GB) */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	float RemainingSpace;

	/* The device that this OS is running on. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BaseDevice* Device;

	/* Reference to the OS widget. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_OsWidget* OsWidget;

	/* Reference to the settings class. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_SystemSettings* SystemSettings;

	/* List of actively running programs. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TMap<int32, class UYetiOS_BaseProgram*> RunningPrograms;

	/* List of installed programs. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<class UYetiOS_BaseProgram*> InstalledPrograms;

	/* The main root directory. Cannot be null. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	mutable class UYetiOS_DirectoryRoot* RootDirectory;

	/* The user that is currently active. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOsUser CurrentActiveUser;

	/* List of all created directories. Used for save game information. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<const UYetiOS_DirectoryBase*> AllCreatedDirectories;

	UPROPERTY()
	class UWorld* OsWorld;

	class FYetiOsNotificationManager* NotificationManager;

	int32 CurrentZOrder;

public:

	UYetiOS_Core();

	static const FString PATH_DELIMITER;
	static const FText ROOT_USER_NAME;

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
	* public static UYetiOS_Core::GetTimeAsText
	* Converts a passed in date & time to a text, formatted as a time using an invariant timezone. This will use the given date & time as-is, so it's assumed to already be in the correct timezone.
	* Returns time in short style. Example: 1:00 PM
	* @param InDateTime [const FDateTime&] Time to convert.
	* @return [const FText] Time as text. Example: 1:00 PM
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	static const FText GetTimeAsText(const FDateTime& InDateTime);

	/**
	* public UYetiOS_Core::CreateOsNotification
	* Creates an OS notification and add it to the notification manager.
	* @param InNewNotification [const FYetiOsNotification] Notification to add.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS")	
	void CreateOsNotification(const FYetiOsNotification InNewNotification);

	/**
	* public UYetiOS_Core::StartOperatingSystemInstallation
	* Starts Operating System installation.
	* @param bShowBsodIfInstallationFails [const bool] If true, then show blue screen widget if installation fails. Recommended to keep this true.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const bool] Returns true if installation was started successfully.
	**/
	const bool StartOperatingSystemInstallation(const bool bShowBsodIfInstallationFails, FYetiOsError& OutErrorMessage);
	
	/**
	* public UYetiOS_Core::LoadOS
	* Load the operating system. Calls BeginLoadOS() in OsWidget.
	**/
	void LoadOS();

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

	void UpdateWindowZOrder(class UYetiOS_DraggableWindowWidget* InWindow);

	/**
	* public UYetiOS_Core::GetInstallablePrograms const
	* Returns a list of pre-defined installable programs.
	* @return [const TArray<TSubclassOf<class UYetiOS_BaseProgram>>] Program classes.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const TArray<TSubclassOf<class UYetiOS_BaseProgram>> GetInstallablePrograms() const { return ProgramsToInstall; }

	/**
	* public UYetiOS_Core::GetOsVersion const
	* Returns the operating system version.
	* @return [const FText] Os version.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")	
	const FText GetOsVersion() const { return OsVersion; }

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
	* protected UYetiOS_Core::GetTotalMemory const
	* Returns total memory on the device.
	* @param bInBytes [const bool] True to return the value in bytes.
	* @return [const float] Total memory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const float GetTotalMemory(const bool bInBytes = true) const;

	/**
	* protected UYetiOS_Core::GetTotalCPUSpeed const
	* Returns the total cpu speed.
	* @param bWithDurability [const bool] If true consider durability to calculate total cpu speed.
	* @return [const float] Returns a combined speed of all cpus.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const float GetTotalCPUSpeed(const bool bWithDurability = true) const;

	/**
	* protected UYetiOS_Core::HasGpuInstalled const
	* Checks if motherboard has at least one GPU installed. This does not check for on board graphics. Only installed GPUs.
	* @return [const bool] True if one or more GPU is available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const bool HasGpuInstalled() const;

	/**
	* protected UYetiOS_Core::HasValidRootDirectoryClass const
	* True if this OS has a valid root directory class.
	* @return [const bool] True if valid root directory class is available.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const bool HasValidRootDirectoryClass() const;
		
	/**
	* protected UYetiOS_Core::GetDeviceScore const
	* Gets a normalized (0-1 range) device score.
	* @return [const float] Range between 0-1.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const float GetDeviceScore() const;
		
	/**
	* protected UYetiOS_Core::GetMainCpu const
	* Returns the primary CPU (at index 0 in motherboard cpus).
	* @return [const FYetiOsCpu] CPU Struct.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const FYetiOsCpu GetMainCpu() const;

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

public:

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
	* @return [const TArray<class UYetiOS_BaseProgram*>] Array of programs installed on this device.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS")
	const TArray<class UYetiOS_BaseProgram*> GetInstalledPrograms() const { return InstalledPrograms; }

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

protected:

	/**
	* protected UYetiOS_Core::K2_OnBatteryLevelChanged
	* Event that is called when battery level changes. Either by depleting or charging.
	* @param bConsumedBattery [const float&] Current battery level.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic, Category = "Yeti OS", DisplayName = "OnBatteryLevelChanged")
	void K2_OnBatteryLevelChanged(const float& CurrentBatteryLevel);

public:
	
	/* Returns an array of compatible device classes for this operating system. */
	FORCEINLINE const TArray<TSubclassOf<class UYetiOS_BaseDevice>> GetCompatibleDeviceClasses() const { return CompatibleDevices; }

	/* Returns the UMG class for OS widget. */
	FORCEINLINE TSubclassOf<class UYetiOS_OsWidget> GetOsWidgetClass() const { return OsWidgetClass; }

	/** Returns the system settings class. */
	FORCEINLINE TSubclassOf<class UYetiOS_SystemSettings> GetSystemSettingsClass() const { return SystemSettingsClass; }

	/* Returns a reference to the Operating system widget created using OsWidgetClass. */
	FORCEINLINE UYetiOS_OsWidget* GetOsWidget() const { return OsWidget; }	

	FORCEINLINE const float GetRemainingSpace() const { return RemainingSpace; }
	FORCEINLINE const TArray<const UYetiOS_DirectoryBase*> GetAllCreatedDirectories() const { return AllCreatedDirectories; }
	FORCEINLINE const FText GetRootCommand() const { return RootCommand; }
	FORCEINLINE class USoundBase* GetNotificationSound(const FYetiOsNotification& InNotification) const
	{
		USoundBase* MySound = nullptr;
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
