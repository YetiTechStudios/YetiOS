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

private:

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	FText DeviceName;

	//UPROPERTY(EditDefaultsOnly)
	//TArray<FYetiOsWiFi> WifiConnections;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	TSubclassOf<class UYetiOS_Core> OperatingSystemClass;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	TSubclassOf<class UYetiOS_DeviceWidget> DeviceWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Base Device")
	TSubclassOf<class UYetiOS_BsodWidget> BsodWidgetClass;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bOperatingSystemInstalled : 1;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bBsodHappened : 1;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	float DeviceScore;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	float MaxDeviceScore;

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

public:

	UYetiOS_BaseDevice();

	template <class T>
	T* GetCastedDevice() { return Cast<T>(this); }

	virtual void OnCreateDevice(FYetiOsError& OutErrorMessage);

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
	* @param bShowBsodOnError [const bool] If true, then show Blue Screen if error happens.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Base Device")	
	void StartOperatingSystem(FYetiOsError& OutErrorMessage, const bool bShowBsodOnError = true);

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

	/**
	* public UYetiOS_BaseDevice::UpdateDeviceState
	* Updates current device state to new state.
	* @param InNewState [EYetiOsDeviceState] State to change to.
	* @return [const bool] True if state was changed.
	**/
	const bool UpdateDeviceState(EYetiOsDeviceState InNewState);

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
	/**
	* private UYetiOS_BaseDevice::Internal_CalculateDeviceScore
	* Calculates the device score. Device Score can be used to check if the device is faster or slower. Higher the number faster the device is.
	**/
	void Internal_CalculateDeviceScore();

	/**
	* private UYetiOS_BaseDevice::Internal_GetSystemDurability const
	* Gets the durability of the system in 0-1 range.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const float] Returns system durability.
	**/
	inline const float Internal_GetSystemDurability(FYetiOsError& OutErrorMessage) const;

	/**
	* private UYetiOS_BaseDevice::Internal_DeviceCanBoot const
	* Checks if the device can boot.
	* @param OutErrorMessage [FYetiOsError &] Outputs error message (if any).
	* @return [const bool] Returns true if device can boot.
	**/
	inline const bool Internal_DeviceCanBoot(FYetiOsError& OutErrorMessage) const;

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
	* Gets an array of physical paths of given file type extension.
	* @param InPath [const FString&] Physical path to search.
	* @param InExtension [const FString&] Extension to search for.
	* @return [const TArray<FString>] Array of files.
	**/
	static const TArray<FString> Internal_GetFiles(const FString& InPath, const FString& InExtension = "*.png");

	/**
	* private static UYetiOS_BaseDevice::Internal_CreatePhysicalDirectory
	* Creates a real physical directory in your system.
	* @param InPath [const FString&] Directory path to create.
	* @return [const bool] True if the directory was created successfully.
	**/
	static const bool Internal_CreatePhysicalDirectory(const FString& InPath);

protected:

	virtual void LoadSavedData(const class UYetiOS_SaveGame* InLoadGameInstance);

	virtual const FYetiOsHardDisk GetHardDisk() const									PURE_VIRTUAL(UYetiOS_BaseDevice::GetHardDisk, return FYetiOsHardDisk(););
	virtual const float GetTotalCpuSpeed(const bool bWithDurability) const				PURE_VIRTUAL(UYetiOS_BaseDevice::GetTotalCpuSpeed, return 0.f;);
	virtual const float GetTotalMemorySize() const										PURE_VIRTUAL(UYetiOS_BaseDevice::GetTotalMemorySize(), return 0.f;);
	virtual const float GetMotherboardDurability() const								PURE_VIRTUAL(UYetiOS_BaseDevice::GetMotherboardDurability, return 0.f;);
	virtual const bool MotherboardHasOnBoardGraphics() const							PURE_VIRTUAL(UYetiOS_BaseDevice::MotherboardHasOnBoardGraphics, return false;);
	virtual const bool CpusAreOfCorrectType(FYetiOsCpu& OutIncorrectCpu) const			PURE_VIRTUAL(UYetiOS_BaseDevice::CpusAreOfCorrectType, return false;);
	virtual const bool IsGpuInstalled() const											PURE_VIRTUAL(UYetiOS_BaseDevice::IsGpuInstalled, return false;);
	virtual const bool HasEnoughPower() const											PURE_VIRTUAL(UYetiOS_BaseDevice::HasEnoughPower, return false;);
	virtual const FString GetSocketName() const											PURE_VIRTUAL(UYetiOS_BaseDevice::GetSocketName, return FString(););
	virtual TSubclassOf<class UYetiOS_DirectoryRoot> GetRootDirectoryClass() const		PURE_VIRTUAL(UYetiOS_BaseDevice::GetRootDirectoryClass, return nullptr;);

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	virtual const TArray<FYetiOsCpu> GetAllCpus() const									PURE_VIRTUAL(UYetiOS_BaseDevice::GetAllCpus, return TArray<FYetiOsCpu>(););

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	virtual const TArray<FYetiOsMemory> GetAllMemory() const							PURE_VIRTUAL(UYetiOS_BaseDevice::GetAllMemory, return TArray<FYetiOsMemory>(););

	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	virtual const TArray<FYetiOsGpu> GetAllGpu() const									PURE_VIRTUAL(UYetiOS_BaseDevice::GetAllGpu, return TArray<FYetiOsGpu>(););

public:

	/**
	* public static UYetiOS_BaseDevice::GetLoginWallpapers
	* Load PNG wallpapers from save directory.
	* @See: Internal_GetLoginWallpapersPath() method.
	* @param InDevice [const UYetiOS_BaseDevice*] You need to pass a valid device.
	* @return [TArray<FString>] An array of wallpaper paths.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")	
	static TArray<FString> GetLoginWallpapers(const UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_BaseDevice::GetDesktopWallpapers
	* Load PNG wallpapers from save directory.
	* @See: Internal_GetDesktopWallpapersPath() method.
	* @param InDevice [const UYetiOS_BaseDevice*] You need to pass a valid device.
	* @return [TArray<FString>] An array of wallpaper paths.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	static TArray<FString> GetDesktopWallpapers(const UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_BaseDevice::GetUserIconImages
	* Load PNG icons from save directory.
	* @See: Internal_UserIconsPath() method.
	* @param InDevice [const UYetiOS_BaseDevice*] You need to pass a valid device.
	* @return [TArray<FString>] An array of icon paths.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")
	static TArray<FString> GetUserIconImages(const UYetiOS_BaseDevice* InDevice);

	/**
	* public static UYetiOS_BaseDevice::CreateTextureFromPath
	* Dynamically loads a texture from given path. Path should be of PNG extension and must be accessible from your physical drive.
	* Example: "C:\Users\YourUsername\Pictures\MyImage.png"
	* @param InImagePath [const FString&] Png image path
	* @param DefaultTextureIfNull [UTexture2D*] Default texture to return if runtime texture fails to load.
	* @return [UTexture2D*] Loaded texture or default texture if runtime texture loading fails.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Base Device")	
	static UTexture2D* CreateTextureFromPath(const FString& InImagePath, UTexture2D* DefaultTextureIfNull);

	FORCEINLINE const bool IsOperatingSystemInstalled() const { return bOperatingSystemInstalled; }
	FORCEINLINE const bool IsInBsodState() const { return bBsodHappened; }
	FORCEINLINE TSubclassOf<class UYetiOS_DeviceWidget> GetDeviceWidgetClass() const { return DeviceWidgetClass; }
	FORCEINLINE const float GetDeviceScore(const bool bNormalize = true) const { return bNormalize ? (DeviceScore / MaxDeviceScore) : DeviceScore; }
	
};
