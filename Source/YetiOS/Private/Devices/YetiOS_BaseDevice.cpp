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
#include "Hardware/YetiOS_Motherboard.h"
#include "Hardware/YetiOS_HardDisk.h"
#include "Misc/DateTime.h"


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
	OperatingSystem = nullptr;
	DeviceWidget = nullptr;
	CurrentDeviceState = EYetiOsDeviceState::STATE_None;
	SaveGameClass = UYetiOS_SaveGame::StaticClass();
	bForceGarbageCollectionWhenDeviceIsDestroyed = false;
}

FText UYetiOS_BaseDevice::GetMonthName(const FDateTime& InDateTime, const bool bShort /*= false*/)
{
	EMonthOfYear Local_CurrentMonth = InDateTime.GetMonthOfYear();
	FString Local_MonthName;
	switch (Local_CurrentMonth)
	{
		case EMonthOfYear::January:
			Local_MonthName = "January";
			break;
		case EMonthOfYear::February:
			Local_MonthName = "February";
			break;
		case EMonthOfYear::March:
			Local_MonthName = "March";
			break;
		case EMonthOfYear::April:
			Local_MonthName = "April";
			break;
		case EMonthOfYear::May:
			Local_MonthName = "May";
			break;
		case EMonthOfYear::June:
			Local_MonthName = "June";
			break;
		case EMonthOfYear::July:
			Local_MonthName = "July";
			break;
		case EMonthOfYear::August:
			Local_MonthName = "August";
			break;
		case EMonthOfYear::September:
			Local_MonthName = "September";
			break;
		case EMonthOfYear::October:
			Local_MonthName = "October";
			break;
		case EMonthOfYear::November:
			Local_MonthName = "November";
			break;
		case EMonthOfYear::December:
			Local_MonthName = "December";
			break;
		default:
			break;
	}

	if (bShort)
	{
		Local_MonthName = Local_MonthName.Mid(0, 3);
	}

	return FText::FromString(Local_MonthName);
}

void UYetiOS_BaseDevice::OnCreateDevice()
{
	UpdateDeviceState(EYetiOsDeviceState::STATE_None);
	DeviceWidget = UYetiOS_DeviceWidget::Internal_CreateDeviceWidget(this);
	ChangeOnScreenWidget(DeviceWidget);
	printlog_veryverbose(FString::Printf(TEXT("Device %s created."), *DeviceName.ToString()));
}

EYetiOsDeviceStartResult UYetiOS_BaseDevice::StartDevice(FYetiOsError& OutErrorMessage)
{
	OutErrorMessage = FYetiOsError();
	printlog(FString::Printf(TEXT("Trying to start %s"), *DeviceName.ToString()));

	DeviceMotherboard = UYetiOS_Motherboard::CreateMotherboard(this, OutErrorMessage);
	if (DeviceMotherboard == nullptr)
	{
		printlog_error(FString::Printf(TEXT("Failed to start %s. Reason: %s"), *DeviceName.ToString(), *OutErrorMessage.ErrorException.ToString()));
		return EYetiOsDeviceStartResult::DEVICESTART_HardwareFail;
	}

	const UYetiOS_SaveGame* LoadGameInstance = UYetiOS_SaveGame::LoadGame(this);
	LoadSavedData(LoadGameInstance);

	if (OperatingSystem == nullptr)
	{
		OperatingSystem = UYetiOS_Core::CreateOperatingSystem(this, OutErrorMessage);

		// If the OS is still null for some reason.
		if (OperatingSystem)
		{
			OperatingSystem->OnOperatingSystemLoadedFromSaveGame(LoadGameInstance, OutErrorMessage);
			
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
	return EYetiOsDeviceStartResult::DEVICESTART_Success;
}

void UYetiOS_BaseDevice::ChangeOnScreenWidget(class UUserWidget* InNewWidget /*= nullptr*/)
{
	if (OnScreenWidget && OnScreenWidget->IsInViewport())
	{
		OnScreenWidget->RemoveFromParent();
	}

	OnScreenWidget = InNewWidget;
	AYetiOS_DeviceManagerActor* OwningDeviceManager = Cast<AYetiOS_DeviceManagerActor>(GetOuter());
	if (OnScreenWidget && OwningDeviceManager->AddWidgetsToScreen())
	{
		OnScreenWidget->AddToViewport();
	}
	
	OwningDeviceManager->OnWidgetChanged(OnScreenWidget);
}

bool UYetiOS_BaseDevice::StartOperatingSystem(FYetiOsError& OutErrorMessage)
{
	return UpdateDeviceState(EYetiOsDeviceState::STATE_Running, OutErrorMessage);
}

void UYetiOS_BaseDevice::ShutdownYetiDevice()
{
	UpdateDeviceState(EYetiOsDeviceState::STATE_PowerOff);
}

void UYetiOS_BaseDevice::RestartYetiDevice()
{
	UpdateDeviceState(EYetiOsDeviceState::STATE_Restart);
}

void UYetiOS_BaseDevice::ShowBSOD(const FText InFaultingModuleName /*= FText::GetEmpty()*/, const FText InExceptionName /*= FText::GetEmpty()*/, const FText InDetailedException /*= FText::GetEmpty()*/)
{
	if (BsodWidget == nullptr && BsodWidgetClass)
	{
		BsodWidget = UYetiOS_BsodWidget::CreateBsodWidget(this, BsodWidgetClass, InFaultingModuleName, InExceptionName, InDetailedException);
	}
	UpdateDeviceState(EYetiOsDeviceState::STATE_BSOD);
}

class AYetiOS_DeviceManagerActor* UYetiOS_BaseDevice::GetDeviceManager() const
{
	return Cast<AYetiOS_DeviceManagerActor>(GetOuter());
}

const bool UYetiOS_BaseDevice::UpdateDeviceState(EYetiOsDeviceState InNewState)
{
	FYetiOsError OutErrorMessage;
	return UpdateDeviceState(InNewState, OutErrorMessage);
}

const bool UYetiOS_BaseDevice::UpdateDeviceState(EYetiOsDeviceState InNewState, FYetiOsError& OutErrorMessage)
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
				{
					ChangeOnScreenWidget(OperatingSystem->GetOsWidget());
					if (OperatingSystem->StartOperatingSystem(bOperatingSystemInstalled, OutErrorMessage) == false)
					{
						UpdateDeviceState(EYetiOsDeviceState::STATE_BSOD);
						return false;
					}
				}
				break;
			case EYetiOsDeviceState::STATE_PowerOff:
				{
					GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
					const bool bSaveSuccess = UYetiOS_SaveGame::SaveGame(this);
					printlog(FString::Printf(TEXT("Save game state: %s"), bSaveSuccess ? *FString("Success!") : *FString("Failed :(")));
					OperatingSystem->ShutdownOS();
					const float TimeToShutdown = FMath::RandRange(1.f, 5.f);
					FTimerHandle TimerHandle_Dummy;
					GetOuter()->GetWorld()->GetTimerManager().SetTimer(TimerHandle_Dummy, this, &UYetiOS_BaseDevice::DestroyYetiDevice, TimeToShutdown, false);
					printlog(FString::Printf(TEXT("%s shuts down in %f seconds."), *DeviceName.ToString(), TimeToShutdown));
				}
				break;
			case EYetiOsDeviceState::STATE_Restart:
				{
					GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
					const bool bSaveSuccess = UYetiOS_SaveGame::SaveGame(this);
					printlog(FString::Printf(TEXT("Save game state: %s"), bSaveSuccess ? *FString("Success!") : *FString("Failed :(")));
					OperatingSystem->RestartOS();
					const float TimeToRestart = FMath::RandRange(1.f, 5.f);
					GetOuter()->GetWorld()->GetTimerManager().SetTimer(TimerHandle_Restart, this, &UYetiOS_BaseDevice::DestroyYetiDeviceAndRestart, TimeToRestart, false);
					printlog(FString::Printf(TEXT("%s restarts in %f seconds."), *DeviceName.ToString(), TimeToRestart));
				}
				break;
			case EYetiOsDeviceState::STATE_BSOD:
				printlog(FString::Printf(TEXT("%s is in error state."), *DeviceName.ToString()));
				{
					ChangeOnScreenWidget(BsodWidget);
				}
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

	// We need to immediately restart so clear TimerHandle_Restart
	printlog(FString::Printf(TEXT("Clear restart timer for %s."), *DeviceName.ToString()));
	GetOuter()->GetWorld()->GetTimerManager().ClearTimer(TimerHandle_Restart);

	if (bOperatingSystemIsPreInstalled)
	{
		UpdateDeviceState(EYetiOsDeviceState::STATE_Running);
	}
	else
	{		
		DestroyYetiDeviceAndRestart();
		ChangeOnScreenWidget(DeviceWidget);
	}
}

void UYetiOS_BaseDevice::DestroyYetiDevice()
{
	Internal_DestroyDevice();
	AYetiOS_DeviceManagerActor* OwningDeviceManager = Cast<AYetiOS_DeviceManagerActor>(GetOuter());	
	OwningDeviceManager->OnCurrentDeviceDestroyed();
}

void UYetiOS_BaseDevice::DestroyYetiDeviceAndRestart()
{
	Internal_DestroyDevice();
	AYetiOS_DeviceManagerActor* OwningDeviceManager = Cast<AYetiOS_DeviceManagerActor>(GetOuter());
	OwningDeviceManager->RestartDevice();
}

void UYetiOS_BaseDevice::Internal_DestroyDevice()
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
}

const bool UYetiOS_BaseDevice::Internal_InstallHardware(class UYetiOS_BaseHardware* InHardware)
{
	if (ensure(InstalledHardwares.Contains(InHardware) == false))
	{
		InstalledHardwares.Add(InHardware);
		if (OperatingSystem)
		{
			static const FText LOCAL_ERR_CODE = LOCTEXT("YetiOS_HW_INSTALL_CODE", "HW_INSTALL_SUCCESS");
			static const FText LOCAL_EXCEPTION = LOCTEXT("YetiOS_HW_INSTALL_TITLE", "Hardware Installed.");

			FFormatNamedArguments Arguments;
			Arguments.Add(TEXT("HardwareName"), InHardware->GetName());
			const FText LOCAL_EXCEPTION_DETAIL = FText::Format(LOCTEXT("YetiOS_HW_INSTALL_DESCRIPTION", "{HardwareName} was installed to the OS."), Arguments);
			FYetiOsNotification HardwareNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_Device, LOCAL_EXCEPTION, LOCAL_EXCEPTION_DETAIL, LOCAL_ERR_CODE, EYetiOsNotificationType::TYPE_Info);
			OperatingSystem->CreateOsNotification(HardwareNotification);
		}
		K2_OnHardwareInstalled(InHardware);
		return true;
	}

	return false;
}

const bool UYetiOS_BaseDevice::Internal_RemoveHardware(class UYetiOS_BaseHardware* InHardware)
{
	if (ensure(InstalledHardwares.Contains(InHardware)))
	{
		InstalledHardwares.RemoveSwap(InHardware);
		K2_OnHardwareRemoved(InHardware);
		return true;
	}

	return false;
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

const UYetiOS_HardDisk* UYetiOS_BaseDevice::GetHardDisk() const
{
	return DeviceMotherboard->GetHardDisk();
}

const float UYetiOS_BaseDevice::GetTotalCpuSpeed() const
{
	return DeviceMotherboard->GetTotalCpuSpeed();
}

const float UYetiOS_BaseDevice::GetTotalMemorySize(const bool bInBytes /*= true*/) const
{
	const float TotalMemory = DeviceMotherboard->GetTotalMemorySize();
	return bInBytes ? TotalMemory * 1000000.f : TotalMemory;
}

TSubclassOf<class UYetiOS_DirectoryRoot> UYetiOS_BaseDevice::GetRootDirectoryClass() const
{
	return DeviceMotherboard->GetHardDisk()->GetRootDirectoryClass();
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
