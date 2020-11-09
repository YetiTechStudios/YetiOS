// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Devices/YetiOS_BaseDevice.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryRoot.h"
#include "Core/YetiOS_SaveGame.h"
#include "Widgets/YetiOS_DeviceWidget.h"
#include "Widgets/YetiOS_BsodWidget.h"
#include "Misc/Paths.h"
#include "HAL/FileManagerGeneric.h"
#include "ImageWrapper/Public/IImageWrapper.h"
#include "ImageWrapper/Public/IImageWrapperModule.h"
#include "Modules/ModuleManager.h"
#include "Engine/Texture2D.h"
#include "Misc/FileHelper.h"
#include "Kismet/KismetMathLibrary.h"
#include "Runtime/Launch/Resources/Version.h"


DEFINE_LOG_CATEGORY_STATIC(LogYetiOsBaseDevice, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsBaseDevice, Log, TEXT("%s"), *FString(Param1))
#define printlog_warn(Param1)			UE_LOG(LogYetiOsBaseDevice, Warning, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsBaseDevice, Error, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsBaseDevice, VeryVerbose, TEXT("%s"), *FString(Param1))

#define CREATE_PHYSICAL_DIR(Func)\
{\
	const FString Local_Path = Func;\
	ensureMsgf(Internal_CreatePhysicalDirectory(Local_Path), TEXT("Failed to create directory %s"), *Local_Path);\
}

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_BaseDevice::UYetiOS_BaseDevice()
{
	DeviceName = FText::GetEmpty();
	bOperatingSystemInstalled = false;
	bBsodHappened = false;
	OperatingSystem = nullptr;
	DeviceWidget = nullptr;
	CurrentDeviceState = EYetiOsDeviceState::STATE_None;

	DeviceScore = 0.f;
	MaxDeviceScore = 0.f;
}

void UYetiOS_BaseDevice::OnCreateDevice()
{
	bBsodHappened = false;
	DeviceWidget = UYetiOS_DeviceWidget::Internal_CreateDeviceWidget(this);
	ChangeOnScreenWidget(DeviceWidget);
	printlog_veryverbose(FString::Printf(TEXT("Device %s created."), *DeviceName.ToString()));
}

EYetiOsDeviceStartResult UYetiOS_BaseDevice::StartDevice(FYetiOsError& OutErrorMessage)
{
	bBsodHappened = false;
	OutErrorMessage = FYetiOsError();
	printlog(FString::Printf(TEXT("Trying to start %s"), *DeviceName.ToString()));

	if (Internal_DeviceCanBoot(OutErrorMessage) == false)
	{
		printlog_error(FString::Printf(TEXT("Failed to start %s. Reason: %s"), *DeviceName.ToString(), *OutErrorMessage.ErrorException.ToString()));
		return EYetiOsDeviceStartResult::DEVICESTART_HardwareFail;
	}

	const UYetiOS_SaveGame* LoadGameInstance = UYetiOS_SaveGame::LoadGame();
	LoadSavedData(LoadGameInstance);

	bool bPartialSuccess = false;
	const float Local_DeviceDurability = Internal_GetSystemDurability(OutErrorMessage);
	if (Local_DeviceDurability < 1.f)
	{
		if (Local_DeviceDurability == 0.f)
		{
			printlog_error(FString::Printf(TEXT("Failed to start %s. Reason: %s"), *DeviceName.ToString(), *OutErrorMessage.ErrorException.ToString()));
			return EYetiOsDeviceStartResult::DEVICESTART_HardwareFail;
		}
		else
		{			
			bPartialSuccess = true;
		}
	}

	Internal_CalculateDeviceScore();
	if (OperatingSystem == nullptr)
	{
		OperatingSystem = UYetiOS_Core::CreateOperatingSystem(this, OutErrorMessage);

		// If the OS is still null for some reason.
		if (OperatingSystem)
		{
			if (LoadGameInstance)
			{
				printlog_veryverbose("Loading OS save data...");
				OperatingSystem->OsVersion = LoadGameInstance->GetOsLoadData().SaveLoad_OSVersion;
				OperatingSystem->OsUsers = LoadGameInstance->GetOsLoadData().SaveLoad_OsUsers;
				OperatingSystem->RemainingSpace = LoadGameInstance->GetOsLoadData().SaveLoad_RemainingSpace;
				if (OperatingSystem->GetRootDirectory())
				{
					TArray<FYetiOsDirectorySaveLoad> SavedDirectories = LoadGameInstance->GetDirectoriesData();
					printlog_veryverbose(FString::Printf(TEXT("Loading %i saved directories..."), SavedDirectories.Num()));
					for (const auto& It : SavedDirectories)
					{
						UYetiOS_DirectoryBase* LoadedDirectory = OperatingSystem->CreateDirectoryInPath(It.SaveLoad_DirPath, It.bSaveLoad_IsHidden, OutErrorMessage, It.SaveLoad_DirectoryName);
						if (LoadedDirectory && It.SaveLoad_Files.Num() > 0)
						{
							printlog_veryverbose(FString::Printf(TEXT("Loading %i file(s) from save data for %s..."), It.SaveLoad_Files.Num(), *LoadedDirectory->GetDirectoryName().ToString()));
							for (const auto& LoadFileIt : It.SaveLoad_Files)
							{
								FYetiOsError Dummy_OutErrorMessage;
								LoadedDirectory->CreateNewFile(LoadFileIt, Dummy_OutErrorMessage);
							}
						}
					}
				}

				TArray<FYetiOsProgramSaveLoad> SavedPrograms = LoadGameInstance->GetProgramData();
				printlog_veryverbose(FString::Printf(TEXT("Loading %i saved programs..."), SavedPrograms.Num()));
				for (const auto& It : SavedPrograms)
				{
					UYetiOS_AppIconWidget* OutWidget;
					OperatingSystem->InstallProgram(It.SaveLoad_ProgramClass, OutErrorMessage, OutWidget);
				}
			}
			
		}
		else
		{
			return EYetiOsDeviceStartResult::DEVICESTART_NoOs;
		}
	}
	
	CREATE_PHYSICAL_DIR(Internal_GetBasePath());
	CREATE_PHYSICAL_DIR(Internal_GetSavePath(this));
	CREATE_PHYSICAL_DIR(Internal_GetLoginWallpapersPath(this));
	CREATE_PHYSICAL_DIR(Internal_GetDesktopWallpapersPath(this));
	CREATE_PHYSICAL_DIR(Internal_UserIconsPath(this));

	UpdateDeviceState(EYetiOsDeviceState::STATE_Starting);
	return bPartialSuccess ? EYetiOsDeviceStartResult::DEVICESTART_PartialSuccess : EYetiOsDeviceStartResult::DEVICESTART_Success;
}

void UYetiOS_BaseDevice::ChangeOnScreenWidget(class UUserWidget* InNewWidget /*= nullptr*/)
{
	if (OnScreenWidget && OnScreenWidget->IsInViewport())
	{
		OnScreenWidget->RemoveFromParent();	
	}

	OnScreenWidget = nullptr;
	AYetiOS_DeviceManagerActor* OwningDeviceManager = Cast<AYetiOS_DeviceManagerActor>(GetOuter());
	if (InNewWidget)
	{
		OnScreenWidget = InNewWidget;
		if (OwningDeviceManager->AddWidgetsToScreen())
		{
			OnScreenWidget->AddToViewport();
		}
	}
	
	OwningDeviceManager->OnWidgetChanged(OnScreenWidget);
}

void UYetiOS_BaseDevice::StartOperatingSystem(FYetiOsError& OutErrorMessage, const bool bShowBsodOnError /*= true*/)
{
	ChangeOnScreenWidget(OperatingSystem->GetOsWidget());
	if (bOperatingSystemInstalled)
	{
		OperatingSystem->LoadOS();
	}
	else
	{
		OperatingSystem->StartOperatingSystemInstallation(bShowBsodOnError, OutErrorMessage);
	}
}

void UYetiOS_BaseDevice::ShutdownYetiDevice()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	const bool bSaveSuccess = UYetiOS_SaveGame::SaveGame(this);
	printlog(FString::Printf(TEXT("Save game state: %s"), bSaveSuccess ? *FString("Success!") : *FString("Failed :(")));
	OperatingSystem->ShutdownOS();
	const float TimeToShutdown = UKismetMathLibrary::MapRangeClamped(GetDeviceScore(true), 0.f, 1.f, 6.f, 3.f);
	FTimerHandle TimerHandle_Dummy;
	GetOuter()->GetWorld()->GetTimerManager().SetTimer(TimerHandle_Dummy, this, &UYetiOS_BaseDevice::DestroyYetiDevice, TimeToShutdown, false);
	printlog(FString::Printf(TEXT("%s shuts down in %f seconds."), *DeviceName.ToString(), TimeToShutdown));
}

void UYetiOS_BaseDevice::RestartYetiDevice()
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	const bool bSaveSuccess = UYetiOS_SaveGame::SaveGame(this);
	printlog(FString::Printf(TEXT("Save game state: %s"), bSaveSuccess ? *FString("Success!") : *FString("Failed :(")));
	OperatingSystem->RestartOS();
	const float TimeToRestart = UKismetMathLibrary::MapRangeClamped(GetDeviceScore(true), 0.f, 1.f, 6.f, 3.f);
	FTimerHandle TimerHandle_Dummy;
	GetOuter()->GetWorld()->GetTimerManager().SetTimer(TimerHandle_Dummy, this, &UYetiOS_BaseDevice::DestroyYetiDeviceAndRestart, TimeToRestart, false);
	printlog(FString::Printf(TEXT("%s restarts in %f seconds."), *DeviceName.ToString(), TimeToRestart));
}

void UYetiOS_BaseDevice::ShowBSOD(const FText InFaultingModuleName /*= FText::GetEmpty()*/, const FText InExceptionName /*= FText::GetEmpty()*/, const FText InDetailedException /*= FText::GetEmpty()*/)
{
	bBsodHappened = true;
	if (BsodWidget == nullptr && BsodWidgetClass)
	{
		BsodWidget = UYetiOS_BsodWidget::CreateBsodWidget(this, BsodWidgetClass, InFaultingModuleName, InExceptionName, InDetailedException);
	}

	ChangeOnScreenWidget(BsodWidget);
}

const bool UYetiOS_BaseDevice::UpdateDeviceState(EYetiOsDeviceState InNewState)
{
	if (CurrentDeviceState != InNewState)
	{
		CurrentDeviceState = InNewState;
		switch (CurrentDeviceState)
		{
			case EYetiOsDeviceState::STATE_Starting:				
				printlog(FString::Printf(TEXT("%s started succesfully."), *DeviceName.ToString()));
				break;
			case EYetiOsDeviceState::STATE_Running:
				printlog(FString::Printf(TEXT("%s changed state to running."), *DeviceName.ToString()));
				break;
			case EYetiOsDeviceState::STATE_PowerOff:
				printlog(FString::Printf(TEXT("%s is shutting down."), *DeviceName.ToString()));
				break;
			case EYetiOsDeviceState::STATE_Restart:
				printlog(FString::Printf(TEXT("%s is restarting..."), *DeviceName.ToString()));
				break;
			default:
				break;
		}
		return true;
	}

	return false;
}

void UYetiOS_BaseDevice::OnFinishInstallingOperatingSystem()
{
	bOperatingSystemInstalled = true;
	printlog(FString::Printf(TEXT("%s installed on %s."), *OperatingSystem->GetOsName().ToString(), *DeviceName.ToString()));
	UpdateDeviceState(EYetiOsDeviceState::STATE_Restart);
	ChangeOnScreenWidget(DeviceWidget);
}

void UYetiOS_BaseDevice::DestroyYetiDevice()
{
	if (OperatingSystem)
	{
		OperatingSystem->DestroyOS();
		OperatingSystem = nullptr;
	}

	DeviceWidget = nullptr;
	BsodWidget = nullptr;
	ChangeOnScreenWidget();
	printlog_veryverbose(FString::Printf(TEXT("Destroyed device '%s'"), *DeviceName.ToString()));
	AYetiOS_DeviceManagerActor* OwningDeviceManager = Cast<AYetiOS_DeviceManagerActor>(GetOuter());	
	OwningDeviceManager->OnCurrentDeviceDestroyed();
	ConditionalBeginDestroy();
}

void UYetiOS_BaseDevice::DestroyYetiDeviceAndRestart()
{
	if (OperatingSystem)
	{
		OperatingSystem->DestroyOS();
		OperatingSystem = nullptr;
	}

	DeviceWidget = nullptr;
	BsodWidget = nullptr;
	ChangeOnScreenWidget();
	printlog_veryverbose(FString::Printf(TEXT("Destroyed device '%s'"), *DeviceName.ToString()));
	AYetiOS_DeviceManagerActor* OwningDeviceManager = Cast<AYetiOS_DeviceManagerActor>(GetOuter());
	OwningDeviceManager->RestartDevice(this);
	ConditionalBeginDestroy();
}

void UYetiOS_BaseDevice::Internal_InstallHardware(class UYetiOS_BaseHardware* InHardware, const bool bForceRemoved)
{
	if (ensure(InstalledHardwares.Contains(InHardware) == false))
	{
		InstalledHardwares.Add(InHardware);
		if (OperatingSystem)
		{
			static const FText LOCAL_ERR_CODE = LOCTEXT("YetiOS_HW_INSTALL_CODE", "HW_INSTALL_SUCCESS");
			static const FText LOCAL_EXCEPTION = LOCTEXT("YetiOS_HW_INSTALL_TITLE", "Hardware Installed.");
			static const FText LOCAL_EXCEPTION_DETAIL = LOCTEXT("YetiOS_HW_INSTALL_DESCRIPTION", "A new hardware was installed to the OS.");
			FYetiOsNotification HardwareNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_Device, LOCAL_EXCEPTION, LOCAL_EXCEPTION_DETAIL, LOCAL_ERR_CODE, EYetiOsNotificationType::TYPE_Info);
			OperatingSystem->CreateOsNotification(HardwareNotification);
		}
	}

	K2_OnHardwareInstalled(InHardware);
}

void UYetiOS_BaseDevice::Internal_RemoveHardware(class UYetiOS_BaseHardware* InHardware)
{
	if (ensure(InstalledHardwares.Contains(InHardware)))
	{
		InstalledHardwares.RemoveSwap(InHardware);
	}

	K2_OnHardwareRemoved(InHardware);
}

void UYetiOS_BaseDevice::Internal_CalculateDeviceScore()
{	
	MaxDeviceScore = 0.f;
	for (auto const& It : GetAllCpus())
	{
		DeviceScore += (It.CpuSpeed * It.CpuDurability);
		MaxDeviceScore += It.GetMaxCPUSpeed();
	}

	for (auto const& It : GetAllMemory())
	{
		DeviceScore += (It.GetMemorySize() * It.MemoryDurability);
		MaxDeviceScore += It.GetMaxMemorySize();
	}

	FYetiOsHardDisk Local_HDD = GetHardDisk();
	DeviceScore += (Local_HDD.HddRpmSpeed * Local_HDD.HddDurability);
	MaxDeviceScore += Local_HDD.HddRpmSpeed;

	DeviceScore = (DeviceScore * GetMotherboardDurability());
}

inline const float UYetiOS_BaseDevice::Internal_GetSystemDurability(FYetiOsError& OutErrorMessage) const
{
	OutErrorMessage = FYetiOsError();
	OutErrorMessage.ErrorCode = LOCTEXT("Device_HardwareDurabilityErrorCode", "FATAL_ERROR_HARDWARE_FAIL");

	TArray<FYetiOsCpu> AllCpus = GetAllCpus();
	TArray<FYetiOsMemory> AllMemories = GetAllMemory();
	TArray<FYetiOsGpu> AllGpus = GetAllGpu();

	const float TotalValue = AllCpus.Num() + AllMemories.Num() + AllGpus.Num() + 2; // + 2 means taking motherboard and HDD durabilities into account.
	const float MotherboardDurability = GetMotherboardDurability();
	const float HddDurability = GetHardDisk().HddDurability;
	float NewDurability = MotherboardDurability + HddDurability;

	if (MotherboardDurability == 0.f)
	{
		OutErrorMessage.ErrorException = LOCTEXT("Device_MotherboardDurabilityErrorException", "Motherboard failed to boot.");
		return 0.f;
	}

	if (HddDurability == 0.f)
	{
		OutErrorMessage.ErrorException = LOCTEXT("Device_HddDurability", "Hard disk failed to boot.");
		return 0.f;
	}

	for (int32 i = 0; i < AllCpus.Num(); ++i)
	{
		const float Durability = AllCpus[i].CpuDurability;
		if (Durability == 0.f)
		{
			OutErrorMessage.ErrorException = FText::Format(LOCTEXT("Device_CpuDurability", "CPU #{0} failed to boot."), FText::AsNumber(i));
			return 0.f;
		}

		NewDurability += Durability;
	}

	for (int32 i = 0; i < AllMemories.Num(); ++i)
	{
		const float Durability = AllMemories[i].MemoryDurability;
		if (Durability == 0.f)
		{
			OutErrorMessage.ErrorException = FText::Format(LOCTEXT("Device_MemoryDurability", "Memory #{0} failed to boot."), FText::AsNumber(i));
			return 0.f;
		}

		NewDurability += Durability;
	}

	for (int32 i = 0; i < AllGpus.Num(); ++i)
	{
		const float Durability = AllGpus[i].GpuDurability;
		if (Durability == 0.f)
		{
			OutErrorMessage.ErrorException = FText::Format(LOCTEXT("Device_GpuDurability", "GPU #{0} failed to boot."), FText::AsNumber(i));
			return 0.f;
		}

		NewDurability += Durability;
	}

	OutErrorMessage.ErrorCode = FText::GetEmpty();
	ensureMsgf(NewDurability <= TotalValue, TEXT("Durability check is incorrect. NewDurability (%f) should be less than TotalValue (%f)"), NewDurability, TotalValue);
	const float TotalDurability = NewDurability / TotalValue;
	static const float DURABILITY_WARNING_THRESHOLD = 0.99;
	if (TotalDurability < DURABILITY_WARNING_THRESHOLD)
	{
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("Device_DurabilityWarning", "CRITICAL WARNING: Hardware durability low. Replace as soon as possible. ({0}%)."), FText::AsNumber(TotalDurability * 100.f));
		printlog_warn(FString::Printf(TEXT("%s durability warning %s"), *DeviceName.ToString(), *OutErrorMessage.ErrorException.ToString()));
	}	

	return TotalDurability;
}

inline const bool UYetiOS_BaseDevice::Internal_DeviceCanBoot(FYetiOsError& OutErrorMessage) const
{
	if (GetAllCpus().IsValidIndex(0) == false)
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Device_BootErrorNoCpuErrorCode", "NO_CPU_BOOT_ERROR");
		OutErrorMessage.ErrorException = LOCTEXT("Device_BootErrorNoCpuErrorException", "You need a cpu to boot this device.");
		return false;
	}
	
	FYetiOsCpu OutIncorrectCpu;
	if (CpusAreOfCorrectType(OutIncorrectCpu) == false)
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Device_BootErrorIncorrectCpuErrorCode", "INCORRECT_CPU_BOOT_ERROR");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("Device_BootErrorIncorrectCpuErrorException", "{0} {1} is of incorrect socket type ({2}). This motherboard requires '{3}' socket type."), OutIncorrectCpu.Brand, OutIncorrectCpu.Model, FText::FromString(OutIncorrectCpu.GetSocketName()), FText::FromString(GetSocketName()));
		return false;
	}

	if (GetAllMemory().IsValidIndex(0) == false)
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Device_BootErrorNoRamErrorCode", "NO_RAM_BOOT_ERROR");
		OutErrorMessage.ErrorException = LOCTEXT("Device_BootErrorNoRamErrorException", "Missing memory.");
		return false;
	}

	if (GetAllGpu().IsValidIndex(0) == false && MotherboardHasOnBoardGraphics() == false)
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Device_BootErrorNoGpuErrorCode", "NO_GRAPHICS_BOOT_ERROR");
		OutErrorMessage.ErrorException = LOCTEXT("Device_BootErrorNoGpuErrorException", "Requires a GPU to boot this device.");
		return false;
	}

	if (HasEnoughPower() == false)
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Device_PowerFailureErrorCode", "POWER_FAILURE");
		OutErrorMessage.ErrorException = LOCTEXT("Device_PowerFailureErrorException", "Your device doesn't have enough power to boot.");
		return false;
	}

	if (GetRootDirectoryClass() == nullptr)
	{
		OutErrorMessage.ErrorCode = LOCTEXT("Device_BootErrorNoRootDirErrorCode", "NO_ROOT_DIRECTORY");
		OutErrorMessage.ErrorException = LOCTEXT("Device_BootErrorNoRootDirErrorException", "Requires a root directory to boot this device.");
		return false;
	}

	return true;
}

const FString UYetiOS_BaseDevice::Internal_GetBasePath()
{
	return FPaths::Combine(FPaths::ProjectSavedDir(), *FString("YetiTechStudios"));
}

const FString UYetiOS_BaseDevice::Internal_GetSavePath(const UYetiOS_BaseDevice* InDevice)
{
	return FPaths::Combine(Internal_GetBasePath(), InDevice->OperatingSystem->GetOsName().ToString().Replace(TEXT(" "), TEXT("")));
}

const FString UYetiOS_BaseDevice::Internal_GetLoginWallpapersPath(const UYetiOS_BaseDevice* InDevice)
{
	return FPaths::Combine(Internal_GetSavePath(InDevice), *FString("LoginWallpapers"));
}

const FString UYetiOS_BaseDevice::Internal_GetDesktopWallpapersPath(const UYetiOS_BaseDevice* InDevice)
{
	return FPaths::Combine(Internal_GetSavePath(InDevice), *FString("DesktopWallpapers"));
}

const FString UYetiOS_BaseDevice::Internal_UserIconsPath(const UYetiOS_BaseDevice* InDevice)
{
	return FPaths::Combine(Internal_GetSavePath(InDevice), *FString("UserIcons"));
}

const TArray<FString> UYetiOS_BaseDevice::Internal_GetFiles(const FString& InPath, const FString& InExtension /*= "*.png"*/)
{
	TArray<FString> Output;
	if (FPaths::DirectoryExists(InPath))
	{
		const FString CheckPath = FString::Printf(TEXT("%s/%s"), *InPath, *InExtension);
		FFileManagerGeneric::Get().FindFiles(Output, *CheckPath, true, false);
		printlog_veryverbose(FString::Printf(TEXT("Found %i %s files in path: %s"), Output.Num(), *InExtension, *InPath));
		for (int i = 0; i < Output.Num(); ++i)
		{
			const FString Result = FString::Printf(TEXT("%s/%s"), *InPath, *Output[i]);
			Output[i] = Result;
			printlog_veryverbose(FString::Printf(TEXT("Found file: %s"), *Result.Replace(*Internal_GetBasePath(), TEXT("..."))));
		}
	}

	return Output;
}

const TArray<FString> UYetiOS_BaseDevice::Internal_GetFiles(const FString& InPath, const TSet<FString>& InExtensions)
{
	TArray<FString> ReturnResult;
	if (FPaths::DirectoryExists(InPath))
	{
		for (const auto& It : InExtensions)
		{
			const FString CheckPath = FString::Printf(TEXT("%s/%s"), *InPath, *It);
			TArray<FString> Output;
			FFileManagerGeneric::Get().FindFiles(Output, *CheckPath, true, false);
			printlog_veryverbose(FString::Printf(TEXT("Found %i %s files in path: %s"), Output.Num(), *It, *InPath));
			for (int i = 0; i < Output.Num(); ++i)
			{
				const FString Result = FString::Printf(TEXT("%s/%s"), *InPath, *Output[i]);
				Output[i] = Result;
				printlog_veryverbose(FString::Printf(TEXT("Found file: %s"), *Result.Replace(*Internal_GetBasePath(), TEXT("..."))));
			}
			ReturnResult.Append(Output);
		}
	}

	return ReturnResult;
}

const bool UYetiOS_BaseDevice::Internal_CreatePhysicalDirectory(const FString& InPath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	
	if (PlatformFile.DirectoryExists(*InPath))
	{
		return true;
	}
	
	printlog_veryverbose(FString::Printf(TEXT("New physical directory created: %s"), *InPath));
	return PlatformFile.CreateDirectory(*InPath);
}

void UYetiOS_BaseDevice::LoadSavedData(const class UYetiOS_SaveGame* InLoadGameInstance)
{
	if (InLoadGameInstance)
	{
		printlog_veryverbose("Loading device save data...");
		bOperatingSystemInstalled = InLoadGameInstance->GetDeviceLoadData().bSaveLoad_OsInstalled;
	}
}

TArray<FString> UYetiOS_BaseDevice::GetLoginWallpapers(const UYetiOS_BaseDevice* InDevice)
{
	return Internal_GetFiles(Internal_GetLoginWallpapersPath(InDevice), GetImageExtensions());
}

TArray<FString> UYetiOS_BaseDevice::GetDesktopWallpapers(const UYetiOS_BaseDevice* InDevice)
{
	return Internal_GetFiles(Internal_GetDesktopWallpapersPath(InDevice), GetImageExtensions());
}

TArray<FString> UYetiOS_BaseDevice::GetUserIconImages(const UYetiOS_BaseDevice* InDevice)
{
	return Internal_GetFiles(Internal_UserIconsPath(InDevice), GetImageExtensions());
}

UTexture2D* UYetiOS_BaseDevice::CreateTextureFromPath(const FString& InImagePath, UTexture2D* DefaultTextureIfNull)
{
	UTexture2D* Texture = nullptr;

	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*InImagePath) == false)
	{
		printlog_error(FString::Printf(TEXT("Image does not exist: %s. Returning default texture..."), *InImagePath));
		return DefaultTextureIfNull;
	}

	TArray<uint8> CompressedData;
	if (FFileHelper::LoadFileToArray(CompressedData, *InImagePath) == false)
	{
		printlog_error(FString::Printf(TEXT("Failed to load image: %s. Returning default texture..."), *InImagePath));
		return DefaultTextureIfNull;
	}

	EImageFormat ImageFormat = EImageFormat::Invalid;
	const FString ImageExtension = FPaths::GetExtension(InImagePath).ToLower();
	if (ImageExtension.IsEmpty())
	{
		printlog_error(FString::Printf(TEXT("Failed to load image: %s (No valid extension). Returning default texture..."), *InImagePath));
		return DefaultTextureIfNull;
	}

	if (ImageExtension == "png")
	{
		ImageFormat = EImageFormat::PNG;
	}
	else if (ImageExtension == "jpg" || ImageExtension == "jpeg")
	{
		ImageFormat = EImageFormat::JPEG;
	}
	else if (ImageExtension == "bmp")
	{
		ImageFormat = EImageFormat::BMP;
	}
	
	if (ImageFormat == EImageFormat::Invalid)
	{
		printlog_error(FString::Printf(TEXT("Failed to load image: %s (Not a valid texture. Supported types are png, jpeg and bmp). Returning default texture..."), *InImagePath));
		return DefaultTextureIfNull;
	}

	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
#if ENGINE_MINOR_VERSION <= 24
		const TArray<uint8>* UncompressedRGBA = nullptr;
#else
		TArray<uint8> UncompressedRGBA;
#endif

		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);

			if (Texture != nullptr)
			{
				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
#if ENGINE_MINOR_VERSION <= 24
				FMemory::Memcpy(TextureData, UncompressedRGBA->GetData(), UncompressedRGBA->Num());
#else
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), Texture->PlatformData->Mips[0].BulkData.GetBulkDataSize());
#endif
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}

	if (Texture == nullptr)
	{
		printlog_warn(FString::Printf(TEXT("Texture was still null. This shouldn't happen! %s"), *InImagePath));
		Texture = DefaultTextureIfNull;
	}

	return Texture;
}

#undef printlog
#undef printlog_warn
#undef printlog_error
#undef printlog_veryverbose
#undef CREATE_PHYSICAL_DIR

#undef LOCTEXT_NAMESPACE
