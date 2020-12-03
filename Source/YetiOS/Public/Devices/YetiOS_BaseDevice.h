// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "YetiOS_BaseDevice.generated.h"

/*************************************************************************
* File Information:
YetiOS_BaseDevice.h

* Description:
A device encapsulates an OS and represents a full product.
*************************************************************************/
UCLASS(Abstract, NotBlueprintable)
class YETIOS_API UYetiOS_BaseDevice : public UObject
{
	GENERATED_BODY()

	friend class UYetiOS_Core;
	friend class UYetiOS_BaseHardware;

	FTimerHandle TimerHandle_Restart;

private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	FText DeviceName;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	FYetiOS_DeviceClasses DeviceClasses;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	TSubclassOf<class UYetiOS_Core> OperatingSystemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	TSubclassOf<class UYetiOS_DeviceWidget> DeviceWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	TSubclassOf<class UYetiOS_BsodWidget> BsodWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device", AdvancedDisplay = "true")
	TSubclassOf<class UYetiOS_SaveGame> SaveGameClass;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bOperatingSystemInstalled : 1;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_Motherboard* DeviceMotherboard;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_Core* OperatingSystem;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_DeviceWidget* DeviceWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BsodWidget* BsodWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	EYetiOsDeviceState CurrentDeviceState;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UUserWidget* OnScreenWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<class UYetiOS_BaseHardware*> InstalledHardwares;

public:

	UYetiOS_BaseDevice();

	UFUNCTION(BlueprintPure, Category = "Yeti Global")
	static FText GetMonthName(const FDateTime& InDateTime);

	/**
	* public UYetiOS_BaseDevice::GetCastedDevice
	* Helper function to return a pointer casted to specific device. Usage example: GetCastedDevice<UYetiOS_PortableDevice>(MyDevice).
	* @return [T*] Returns device casted to specific class.
	**/
	template <class T>
	T* GetCastedDevice() 
	{ 
		static_assert(TIsDerivedFrom<T, UYetiOS_BaseDevice>::IsDerived, "GetCastedDevice() can only be used on classes derived from UYetiOS_BaseDevice.");
		return Cast<T>(this);
	}

	/**
	* virtual public UYetiOS_BaseDevice::OnCreateDevice
	* Called after the device is constructed.
	**/
	virtual void OnCreateDevice();

	/**
	* virtual public UYetiOS_BaseDevice::StartDevice
	* Starts this device.
	* @param OutErrorMessage [FText&] Output of any error message. if return result is success, this will be empty.
	* @return [EDeviceStartResult] Returns Device start result.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Device")
	virtual EYetiOsDeviceStartResult StartDevice(FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_BaseDevice::ChangeOnScreenWidget
	* Changes the on screen widget.
	* @param InNewWidget [class UUserWidget*] New widget to add.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Device")	
	void ChangeOnScreenWidget(class UUserWidget* InNewWidget = nullptr);

	/**
	* public UYetiOS_BaseDevice::StartOperatingSystem
	* Starts the operating system. This will detect if the OS is installed or no. If installed, then it will start. If not, it will install it.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Device")	
	bool StartOperatingSystem(FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_BaseDevice::ShutdownYetiDevice
	* Shuts down this device.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Device", DisplayName = "Shutdown Device")	
	virtual void ShutdownYetiDevice();

	/**
	* virtual public UYetiOS_BaseDevice::RestartYetiDevice
	* Restarts this device.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Device", DisplayName = "Restart Device")	
	virtual void RestartYetiDevice();

	/**
	* public UYetiOS_BaseDevice::ShowBSOD
	* Shows the Blue Screen widget.	
	* @param InFaultingModuleName [const FText] A fake module name. Can be error code also (if any).
	* @param InExceptionName [const FText] Exception name (if any).
	* @param InDetailedException [const FText] Detailed exception name (if any).
	**/
	void ShowBSOD(const FText InFaultingModuleName = FText::GetEmpty(), const FText InExceptionName = FText::GetEmpty(), const FText InDetailedException = FText::GetEmpty());

	/**
	* public UYetiOS_BaseDevice::IsPortableDevice const
	* Checks if this device is a portable device or not. Portable devices has battery level, charging etc.
	* @return [bool] True if this is a portable device.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	virtual const bool IsPortableDevice() const { return false; }

	/**
	* public UYetiOS_BaseDevice::GetOperatingSystem const
	* Gets the current operating system.
	* @return [class UYetiOS_Core*] Returns OperatingSystem.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")	
	inline class UYetiOS_Core* GetOperatingSystem() const { return OperatingSystem; }

	/**
	* public UYetiOS_BaseDevice::GetDeviceWidget const
	* Gets the current device widget.
	* @return [class UYetiOS_DeviceWidget*] Returns DeviceWidget.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	inline class UYetiOS_DeviceWidget* GetDeviceWidget() const { return DeviceWidget; }

	/**
	* public UYetiOS_BaseDevice::GetDeviceName const
	* Gets the name of the device.
	* @return [FText] Returns device name.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")	
	inline FText GetDeviceName() const { return DeviceName; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	class UYetiOS_Motherboard* GetMotherboard() const { return DeviceMotherboard; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	class AYetiOS_DeviceManagerActor* GetDeviceManager() const;

	/**
	* public UYetiOS_BaseDevice::UpdateDeviceState
	* Updates current device state to new state.
	* @param InNewState [EYetiOsDeviceState] State to change to.
	* @return [const bool] True if state was changed.
	**/
	const bool UpdateDeviceState(EYetiOsDeviceState InNewState);
	const bool UpdateDeviceState(EYetiOsDeviceState InNewState, FYetiOsError& OutErrorMessage);

	/**
	* virtual public UYetiOS_BaseDevice::OnFinishInstallingOperatingSystem
	* Called when operating system finishes its installation.
	* @See: UYetiOS_Core::Internal_FinishOperatingSystemInstallation()
	**/
	virtual void OnFinishInstallingOperatingSystem();

	/**
	* virtual public UYetiOS_BaseDevice::DestroyYetiDevice
	* Destroys this device.
	* @See: UYetiOS_BaseDevice::ShutdownYetiDevice()
	* @See: AYetiOS_DeviceManagerActor::EndPlay
	**/
	virtual void DestroyYetiDevice();

	/**
	* virtual public UYetiOS_BaseDevice::DestroyYetiDeviceAndRestart
	* Destroys this device and restart.
	* @See: UYetiOS_BaseDevice::RestartYetiDevice()
	**/
	virtual void DestroyYetiDeviceAndRestart();

private:

	void Internal_DestroyDevice();
	void Internal_InstallHardware(class UYetiOS_BaseHardware* InHardware, const bool bForceRemoved);
	void Internal_RemoveHardware(class UYetiOS_BaseHardware* InHardware);

	/**
	* private static UYetiOS_BaseDevice::Internal_GetBasePath
	* Gets the physical save path directory.
	* @return [const FString] Save path directory.
	**/
	static const FString Internal_GetBasePath();

	static const FString Internal_GetSavePath(const UYetiOS_BaseDevice* InDevice);
	static const FString Internal_GetLoginWallpapersPath(const UYetiOS_BaseDevice* InDevice);
	static const FString Internal_GetDesktopWallpapersPath(const UYetiOS_BaseDevice* InDevice);
	static const FString Internal_UserIconsPath(const UYetiOS_BaseDevice* InDevice);

	/**
	* private static UYetiOS_BaseDevice::Internal_GetFiles
	* Gets an array of physical paths of given file type extensions.
	* @param InPath [const FString&] Physical path to search.
	* @param InExtensions [const TSet<FString>&] Array of Extensions to search for. Example *.png, *.jpg etc. @See GetImageExtensions method.
	* @return [const TArray<FString>] Array of files.
	**/
	static const TArray<FString> Internal_GetFiles(const FString& InPath, const TSet<FString>& InExtensions);

	/**
	* private static UYetiOS_BaseDevice::Internal_CreatePhysicalDirectory
	* Creates a real physical directory in your system.
	* @param InPath [const FString&] Directory path to create.
	* @return [const bool] True if the directory was created successfully.
	**/
	static const bool Internal_CreatePhysicalDirectory(const FString& InPath);

protected:

	virtual void LoadSavedData(const class UYetiOS_SaveGame* InLoadGameInstance);

	virtual const UYetiOS_HardDisk* GetHardDisk() const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	virtual const float GetTotalCpuSpeed() const; 

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	virtual const float GetTotalMemorySize(const bool bInBytes = true) const;

	virtual TSubclassOf<class UYetiOS_DirectoryRoot> GetRootDirectoryClass() const;

public:

	/**
	* public static UYetiOS_BaseDevice::GetLoginWallpapers
	* Load PNG wallpapers from save directory.
	* @See: Internal_GetLoginWallpapersPath() method.
	* @param InDevice [const UYetiOS_BaseDevice*] You need to pass a valid device.
	* @return [TArray<FString>] An array of wallpaper paths.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")	
	static TArray<FString> GetLoginWallpapers(const UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_BaseDevice::GetDesktopWallpapers
	* Load PNG wallpapers from save directory.
	* @See: Internal_GetDesktopWallpapersPath() method.
	* @param InDevice [const UYetiOS_BaseDevice*] You need to pass a valid device.
	* @return [TArray<FString>] An array of wallpaper paths.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")
	static TArray<FString> GetDesktopWallpapers(const UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_BaseDevice::GetUserIconImages
	* Load PNG icons from save directory.
	* @See: Internal_UserIconsPath() method.
	* @param InDevice [const UYetiOS_BaseDevice*] You need to pass a valid device.
	* @return [TArray<FString>] An array of icon paths.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")
	static TArray<FString> GetUserIconImages(const UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_BaseDevice::CreateTextureFromPath
	* Dynamically loads a texture from given path. Path should be of PNG extension and must be accessible from your physical drive.
	* Example: "C:\Users\YourUsername\Pictures\MyImage.png"
	* @param InImagePath [const FString&] Png image path
	* @param DefaultTextureIfNull [UTexture2D*] Default texture to return if runtime texture fails to load.
	* @return [UTexture2D*] Loaded texture or default texture if runtime texture loading fails.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti Global")	
	static UTexture2D* CreateTextureFromPath(const FString& InImagePath, UTexture2D* DefaultTextureIfNull);

protected:

	/**
	* protected UYetiOS_BaseDevice::K2_OnHardwareInstalled
	* Event called when a hardware is installed.
	* @param InstalledHardware [class UYetiOS_BaseHardware*] Newly installed hardware.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Device", DisplayName = "On Hardware Installed")	
	void K2_OnHardwareInstalled(class UYetiOS_BaseHardware* InstalledHardware);

	/**
	* protected UYetiOS_BaseDevice::K2_OnHardwareRemoved
	* Event called when a hardware is removed.
	* @param RemovedHardware [class UYetiOS_BaseHardware*] Removed hardware.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Base Device", DisplayName = "On Hardware Removed")	
	void K2_OnHardwareRemoved(class UYetiOS_BaseHardware* RemovedHardware);

public:

	FORCEINLINE const bool IsOperatingSystemInstalled() const { return bOperatingSystemInstalled; }
	FORCEINLINE const bool IsInBsodState() const { return CurrentDeviceState == EYetiOsDeviceState::STATE_BSOD; }
	FORCEINLINE TSubclassOf<class UYetiOS_DeviceWidget> GetDeviceWidgetClass() const { return DeviceWidgetClass; }
	FORCEINLINE const FYetiOS_DeviceClasses& GetDeviceClasses() const { return DeviceClasses; }
	FORCEINLINE TSubclassOf<class UYetiOS_SaveGame> GetSaveGameClass() const { return SaveGameClass; }
	
	static FORCEINLINE const TSet<FString> GetImageExtensions()
	{
		static TSet<FString> Output;
		Output.Add("*.png");
		Output.Add("*.jpg");
		Output.Add("*.jpeg");
		Output.Add("*.bmp");
		return Output;
	}
};
