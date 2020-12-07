// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Core/YetiOS_DirectoryRoot.h"
#include "Core/YetiOS_SaveGame.h"
#include "Core/YetiOS_Taskbar.h"
#include "Core/YetiOS_StartMenu.h"
#include "Misc/YetiOS_SystemSettings.h"
#include "Devices/YetiOS_PortableDevice.h"
#include "Devices/YetiOS_StationaryDevice.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Widgets/YetiOS_OsWidget.h"
#include "Widgets/YetiOS_AppIconWidget.h"
#include "Widgets/YetiOS_DraggableWindowWidget.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/Public/TimerManager.h"
#include "Runtime/UMG/Public/Components/CanvasPanelSlot.h"
#include "Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h"
#include "Hardware/YetiOS_Motherboard.h"
#include "Hardware/YetiOS_HardDisk.h"
#include "Core/YetiOS_FileBase.h"
#include "Misc/YetiOS_ProgramsRepository.h"
#include "Widgets/YetiOS_DialogWidget.h"
#include "Core/YetiOS_BaseDialogProgram.h"


DEFINE_LOG_CATEGORY_STATIC(LogYetiOsOperatingSystem, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsOperatingSystem, Log, TEXT("%s"), *FString(Param1))
#define printlog_warn(Param1)			UE_LOG(LogYetiOsOperatingSystem, Warning, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsOperatingSystem, Error, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsOperatingSystem, VeryVerbose, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

static const int32 MIN_PROCES_ID_TO_GENERATE = 1;
static const int32 MAX_PROCES_ID_TO_GENERATE = 99999;

const FString UYetiOS_Core::PATH_DELIMITER = "/";

static const FText INSTALL_ERROR_CODE = LOCTEXT("YetiOS_InstallProgramErrorCode", "ERR_INSTALL_FAIL");
static const FText CLOSE_ERROR_CODE = LOCTEXT("YetiOS_CloseProgramErrorCode", "ERR_CLOSE_PROGRAM");
static const FText RUN_ERROR_CODE = LOCTEXT("YetiOS_RunProgramInstanceErrorCode", "ERR_RUN_PROGRAM");

UYetiOS_Core::UYetiOS_Core()
{
	OsName = FText::GetEmpty();
	OsVersion = FYetiOS_Version(1, 0, 0);
	OsIcon = nullptr;
	InstallationSpaceInMB = 10000.f;
	ReleaseState = EYetiOsOperatingSystemReleaseState::STATE_FullRelease;
	MinInstallationTime = 10.f;
	MaxInstallationTime = 60.f;

	CurrentZOrder = INDEX_NONE;

	RootUser = FYetiOsUser("root");
	RootCommand = FText::AsCultureInvariant("sudo");
}

UYetiOS_Core* UYetiOS_Core::CreateOperatingSystem(class UYetiOS_BaseDevice* InParentDevice, FYetiOsError& OutErrorMessage)
{
	if (InParentDevice->OperatingSystemClass)
	{
		UYetiOS_Core* ProxyOS = NewObject<UYetiOS_Core>(InParentDevice, InParentDevice->OperatingSystemClass);
		if (ProxyOS->GetCompatibleDeviceClasses().Contains(InParentDevice->GetClass()))
		{
			if (ProxyOS->SystemSettings)
			{
				if (ProxyOS->SystemSettings->ColorCollections.IsValidIndex(0))
				{
					ProxyOS->SystemSettings->CurrentCollectionName = ProxyOS->SystemSettings->ColorCollections[0].CollectionName;
				}
			}

			ProxyOS->Device = InParentDevice;
			ProxyOS->OsWorld = InParentDevice->GetWorld();
			ProxyOS->bIsPreInstalled = InParentDevice->IsOperatingSystemPreInstalled();
			ProxyOS->OsWidget = UYetiOS_OsWidget::Internal_CreateOsWidget(ProxyOS);
			ProxyOS->Taskbar = UYetiOS_Taskbar::CreateTaskbar(ProxyOS);
			ProxyOS->NotificationManager = FYetiOsNotificationManager::CreateNotificationManager();
			ProxyOS->InstalledPrograms.Empty();
			return ProxyOS;
		}

		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_CreateOperatingSystemErrorCode", "INCOMPATIBLE_OPERATING_SYSTEM");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_CreateOperatingSystemErrorException", "{0} is not compatible with this device."), ProxyOS->GetOsName());
		ProxyOS->ConditionalBeginDestroy();
		return nullptr;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_NullOperatingSystemErrorCode", "NO_VALID_OPERATING_SYSTEM");
	OutErrorMessage.ErrorException = LOCTEXT("YetiOS_NullOperatingSystemErrorException", "Operating system class is NULL.");
	return nullptr;
}

const bool UYetiOS_Core::CloseDialogWidget(UYetiOS_Core* InOS, UYetiOS_DialogWidget* InDialogWidget)
{
	if (InOS && InDialogWidget)
	{
		InOS->CurrentDialogWidgets.RemoveSwap(InDialogWidget);
		InDialogWidget->RemoveFromParent();
		return true;
	}

	return false;
}

class UYetiOS_DialogWidget* UYetiOS_Core::OpenDialogWidget(UYetiOS_Core* InOS, TSubclassOf<class UYetiOS_DialogWidget> InDialogWidgetClass, TSubclassOf<class UYetiOS_BaseDialogProgram> DialogClass, const FText& InMessage, FText InTitle /*= INVTEXT("Dialog")*/, const FVector2D& OverrideWindowSize /*= FVector2D::ZeroVector*/, const bool bIsModalDialog /*= true*/, EYetiOS_DialogType InDialogType /*= EYetiOS_DialogType::Ok*/)
{
	if (DialogClass)
	{
		FYetiOsError OutError;
		UYetiOS_BaseProgram* Local_InstalledProgram = nullptr;
		UYetiOS_BaseDialogProgram* Local_InstalledDialogProgram = nullptr;
		if (InOS->IsProgramInstalled(DialogClass->GetDefaultObject<UYetiOS_BaseDialogProgram>()->GetProgramIdentifierName(), Local_InstalledProgram, OutError))
		{
			Local_InstalledDialogProgram = Cast<UYetiOS_BaseDialogProgram>(Local_InstalledProgram);
		}
		else
		{
			UYetiOS_AppIconWidget* OutIcon;
			Local_InstalledDialogProgram = Cast<UYetiOS_BaseDialogProgram>(InOS->InstallProgram(DialogClass, OutError, OutIcon));
		}

		UYetiOS_DialogWidget* ProxyDialog = UYetiOS_DialogWidget::Internal_CreateDialogWidget(InOS, Local_InstalledDialogProgram, InDialogWidgetClass, bIsModalDialog, InDialogType);
		if (ProxyDialog)
		{
			if (OverrideWindowSize.IsZero() == false)
			{
				Local_InstalledDialogProgram->SetOverrideWindowSize(OverrideWindowSize);
			}

			Local_InstalledDialogProgram->StartProgram(OutError);
			Local_InstalledDialogProgram->Internal_SetDialogWidget(ProxyDialog);
			UYetiOS_DraggableWindowWidget* Local_OwningWindow = Local_InstalledDialogProgram->GetOwningWindow();
			Local_OwningWindow->AddWidget(ProxyDialog);
			Local_OwningWindow->BringWindowToFront();
			InOS->CurrentDialogWidgets.Add(ProxyDialog);
			ProxyDialog->K2_OnSetTitle(InTitle);
			ProxyDialog->K2_OnSetMessage(InMessage);
			return ProxyDialog;
		}
	}

	return nullptr;
}

bool UYetiOS_Core::EvaluateMathExpression(const FString& InExpression, float& OutValue)
{
	return FMath::Eval(InExpression, OutValue);
}

TArray<class UYetiOS_DirectoryBase*> UYetiOS_Core::GetSystemDirectories(class UYetiOS_Core* InOS)
{
	if (InOS && InOS->RootDirectory)
	{
		return InOS->RootDirectory->GetSystemDirectories();
	}

	return TArray<UYetiOS_DirectoryBase*>();
}

const FText UYetiOS_Core::GetTimeAsText(const FDateTime& InDateTime, EYetiOSTimeFormat TimeFormat /*= EYetiOSTimeFormat::Medium*/)
{
	EDateTimeStyle::Type DateTimeType = EDateTimeStyle::Default;
	switch (TimeFormat)
	{
		case EYetiOSTimeFormat::Short:
			DateTimeType = EDateTimeStyle::Short;
			break;
		case EYetiOSTimeFormat::Medium:
			DateTimeType = EDateTimeStyle::Medium;
			break;
		case EYetiOSTimeFormat::Long:
			DateTimeType = EDateTimeStyle::Long;
			break;
		case EYetiOSTimeFormat::Full:
			DateTimeType = EDateTimeStyle::Full;
			break;
		default:
			break;
	}
	return FText::AsTime(InDateTime, DateTimeType, FText::GetInvariantTimeZone());
}

bool UYetiOS_Core::GetColorCollectionOfTheme(class UYetiOS_Core* InOS, EYetiOsThemeMode InTheme, FYetiOsColorCollection& OutCollection)
{
	if (InOS)
	{
		UYetiOS_SystemSettings* ProxySettings = InOS->GetSystemSettings();
		if (ProxySettings)
		{
			OutCollection = ProxySettings->GetColorCollectionFromTheme(InTheme);
			return true;
		}
	}

	return false;
}

FLinearColor UYetiOS_Core::GetColorFromCurrent(class UYetiOS_Core* InOS, EYetiOsColorTypes InColorType)
{
	if (InOS)
	{
		UYetiOS_SystemSettings* ProxySettings = InOS->GetSystemSettings();
		if (ProxySettings)
		{
			return ProxySettings->GetColorFromCurrent(InColorType);
		}
	}

	return FLinearColor::Transparent;
}

void UYetiOS_Core::CreateOsNotification(const FYetiOsNotification InNewNotification)
{
	if (NotificationSettings.bEnableNotifications)
	{
		OsWidget->ReceiveNotification(InNewNotification);
		NotificationManager->LogNotification(InNewNotification);
	}
}

const bool UYetiOS_Core::StartOperatingSystem(const bool bIsInstalled, FYetiOsError& OutErrorMessage)
{
	bool bSuccess = false;

	UYetiOS_DirectoryRoot* MyRootDirectory = GetRootDirectory();
	if (MyRootDirectory)
	{
		DesktopDirectory = MyRootDirectory->GetChildDirectoryByType(EDirectoryType::Desktop);
		if (DesktopDirectory.IsValid())
		{
			if (bIsInstalled)
			{
				OsWidget->BeginLoadOS();
				bSuccess = true;
			}
			else
			{
				if (GetOwningDevice()->GetMotherboard()->GetHardDisk()->ConsumeSpace(InstallationSpaceInMB))
				{
					if (bIsPreInstalled)
					{
						Internal_FinishOperatingSystemInstallation();
					}
					else
					{
						static const FText Title = LOCTEXT("YetiOS_StartInstallation", "Begin Installation.");
						static const FText Description = LOCTEXT("YetiOS_StartInstallationDescription", "Operating system installation started on device.");
						static const FText Code = LOCTEXT("YetiOS_StartInstallationCode", "INSTALL_START");
						const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_Device, Title, Description, Code);
						CreateOsNotification(NewNotification);

						CalculatedInstallationTime = FMath::RandRange(MinInstallationTime, MaxInstallationTime);

						printlog(FString::Printf(TEXT("%s Will finish in %f seconds."), *Description.ToString(), CalculatedInstallationTime));
						OsWidget->StartOsInstallation(CalculatedInstallationTime);

						OsWorld->GetTimerManager().SetTimer(TimerHandle_OsInstallation, this, &UYetiOS_Core::Internal_FinishOperatingSystemInstallation, CalculatedInstallationTime, false);
					}

					bSuccess = true;
				}
				else
				{

					OutErrorMessage.ErrorCode = YetiOS_CommonErrors::OsInstallCode;
					OutErrorMessage.ErrorException = YetiOS_CommonErrors::OsInstallException;
					OutErrorMessage.ErrorDetailedException = YetiOS_CommonErrors::OsInstallDetailedException;
					
				}
			}
		}
		else
		{
			OutErrorMessage.ErrorCode = YetiOS_CommonErrors::OsDesktopCode;
			OutErrorMessage.ErrorException = YetiOS_CommonErrors::OsDesktopException;
			OutErrorMessage.ErrorDetailedException = YetiOS_CommonErrors::OsDesktopDetailedException;
		}
	}
	else
	{
		OutErrorMessage.ErrorCode = YetiOS_CommonErrors::OsRootCode;
		OutErrorMessage.ErrorException = YetiOS_CommonErrors::OsRootException;
		OutErrorMessage.ErrorDetailedException = YetiOS_CommonErrors::OsRootDetailedException;
	}

	if (bSuccess == false)
	{
		printlog_error(FString::Printf(TEXT("OS Name: %s - %s"), *OsName.ToString(), *OutErrorMessage.ErrorDetailedException.ToString()));
		AYetiOS_DeviceManagerActor::ShowBSOD(this, Device, OutErrorMessage);
	}

	return bSuccess;
}

void UYetiOS_Core::ShutdownOS()
{
	TArray<UYetiOS_BaseProgram*> ProgramsArray;
	RunningPrograms.GenerateValueArray(ProgramsArray);
	for (const auto& It : ProgramsArray)
	{
		FYetiOsError OutError;
		It->CloseProgram(OutError, true);
	}

	OsWidget->BeginShutdownOS();
}

void UYetiOS_Core::RestartOS()
{
	TArray<UYetiOS_BaseProgram*> ProgramsArray;
	RunningPrograms.GenerateValueArray(ProgramsArray);
	for (const auto& It : ProgramsArray)
	{
		FYetiOsError OutError;
		It->CloseProgram(OutError, true);
	}

	OsWidget->BeginRestartOS();
}

bool UYetiOS_Core::AddNewUser(FYetiOsUser InNewUser, FYetiOsError& OutErrorMessage)
{
	if (InNewUser.UserName.EqualToCaseIgnored(RootUser.UserName))
	{
		const FText Title = LOCTEXT("YetiOS_InvalidUsernameErrorText", "Restricted Username");
		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_InvalidUsernameError", "ERR_INVALID_USERNAME");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_InvalidUsernameErrorException", "You cannot use '{0}' as your username."), RootUser.UserName);
		OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_InvalidUsernameErrorException", "'{0}' is a system reserved username. Please try again with another name."), RootUser.UserName);
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);
		return false;
	}
	
	OsUsers.Add(InNewUser);
	return true;
}

UYetiOS_BaseProgram* UYetiOS_Core::InstallProgram(TSubclassOf<UYetiOS_BaseProgram> InProgramToInstall, FYetiOsError& OutErrorMessage, UYetiOS_AppIconWidget*& OutIconWidget)
{
	const UYetiOS_BaseProgram* DefaultConstructed = InProgramToInstall->GetDefaultObject<UYetiOS_BaseProgram>();
	const FText MyProgramName = DefaultConstructed->GetProgramName();
	if (Device->IsInBsodState())
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_InstallProgramBsodError", "Cannot install {0}."), MyProgramName);
		static const FText Description = LOCTEXT("YetiOS_InstallProgramBsodErrorDescription", "Cannot install program. System unstable.");		
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, INSTALL_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = INSTALL_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		printlog_error(Description.ToString());
		return nullptr;
	}

	for (const auto& It : InstalledPrograms)
	{
		if (It->GetClass() == InProgramToInstall)
		{
			const FText Title = FText::Format(LOCTEXT("YetiOS_InstallProgramError", "{0} already installed."), MyProgramName);
			const FText Description = FText::Format(LOCTEXT("YetiOS_InstallProgramErrorDescription", "{0} is already installed. Uninstall the existing app to install {0} again."), MyProgramName);			
			const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, INSTALL_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
			CreateOsNotification(NewNotification);

			OutErrorMessage.ErrorCode = INSTALL_ERROR_CODE;
			OutErrorMessage.ErrorException = Title;
			OutErrorMessage.ErrorDetailedException = Description;

			printlog_error(Description.ToString());
			return nullptr;
		}
	}

	if (GetOwningDevice()->GetMotherboard()->GetHardDisk()->HasEnoughSpace(DefaultConstructed->GetProgramSpace()) == false)
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_InstallProgramNoSpaceError", "Not enough space for {0}."), MyProgramName);
		const FText Description = FText::Format(LOCTEXT("YetiOS_InstallProgramNoSpaceErrorDescription", "Not enough space to install {0}. Free up space by uninstalling existing apps or expand your storage."), MyProgramName);		
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, INSTALL_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = INSTALL_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		printlog_error(Description.ToString());
		return nullptr;
	}

	const bool bIsPredefinedProgram = ProgramsRepository->IsInstalledWithOS(InProgramToInstall);
	UYetiOS_BaseProgram* NewProgram = UYetiOS_BaseProgram::CreateProgram(this, InProgramToInstall, OutErrorMessage, bIsPredefinedProgram);
	if (NewProgram)
	{
		GetOwningDevice()->GetMotherboard()->GetHardDisk()->ConsumeSpace(NewProgram->GetProgramSpace());
		OutIconWidget = UYetiOS_AppIconWidget::CreateProgramIconWidget(NewProgram, OutErrorMessage);
		InstalledPrograms.Add(NewProgram);
		printlog(FString::Printf(TEXT("Program %s installed."), *NewProgram->GetProgramName().ToString()));
		if (NewProgram->CanAddToDesktop())
		{
			OsWidget->AddIconWidgetToDesktop(OutIconWidget);
		}
		
		if (NewProgram->CanShowPostInstallNotification())
		{
			const FText Title = LOCTEXT("YetiOS_InstallProgramSuccess", "Installed Program.");
			const FText Description = FText::Format(LOCTEXT("YetiOS_InstallProgramUnknownErrorDescription", "{0} installed successfuly."), MyProgramName);
			const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, FText::FromString("INSTALL_SUCCESS"), EYetiOsNotificationType::TYPE_Info);
			CreateOsNotification(NewNotification);
		}

		NewProgram->ProgramInstalled();
		OnProgramInstalled.Broadcast(NewProgram);
		return NewProgram;
	}
	
	const FText Title = FText::Format(LOCTEXT("YetiOS_InstallProgramUnknownError", "Cannot install {0}."), MyProgramName);
	const FText Description = FText::Format(LOCTEXT("YetiOS_InstallProgramUnknownErrorDescription", "{0} cannot be installed. Null program."), MyProgramName);	
	const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, INSTALL_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
	CreateOsNotification(NewNotification);

	OutErrorMessage.ErrorCode = INSTALL_ERROR_CODE;
	OutErrorMessage.ErrorException = Title;
	OutErrorMessage.ErrorDetailedException = Description;
	
	printlog_error(Description.ToString());
	return nullptr;
}

UYetiOS_BaseProgram* UYetiOS_Core::InstallProgramFromPackage(const FString& InProgramIdentifier, FYetiOsError& OutErrorMessage, UYetiOS_AppIconWidget*& OutIconWidget)
{
	OutIconWidget = nullptr;

	TSubclassOf<UYetiOS_BaseProgram> ProgramClassToInstall = Internal_FindProgramFromPackage(FName(*InProgramIdentifier));
	if (ProgramClassToInstall)
	{
		return InstallProgram(ProgramClassToInstall, OutErrorMessage, OutIconWidget);
	}

	const FText Title = FText::Format(LOCTEXT("YetiOS_InstallProgramPackageError", "Failed to find package {0}"), FText::FromString(InProgramIdentifier));
	const FText Description = FText::Format(LOCTEXT("YetiOS_InstallProgramPackageErrorDescription", "Cannot install program {0}. Not found in repo."), FText::FromString(InProgramIdentifier));
	const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, INSTALL_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
	CreateOsNotification(NewNotification);
	
	OutErrorMessage.ErrorCode = INSTALL_ERROR_CODE;
	OutErrorMessage.ErrorException = Title;
	OutErrorMessage.ErrorDetailedException = Description;	
	return nullptr;
}

const bool UYetiOS_Core::IsProgramRunning(const class UYetiOS_BaseProgram* InProgram) const
{
	TArray<UYetiOS_BaseProgram*> OutArray;
	RunningPrograms.GenerateValueArray(OutArray);
	for (const auto& It : OutArray)
	{
		if (It == InProgram)
		{
			return true;
		}
	}

	return false;
}

const bool UYetiOS_Core::IsProgramRunning(const FName& InProgramIdentifier, class UYetiOS_BaseProgram*& OutProgram) const
{
	OutProgram = nullptr;
	TArray<UYetiOS_BaseProgram*> OutArray;
	RunningPrograms.GenerateValueArray(OutArray);
	for (const auto& It : OutArray)
	{
		if (It->GetProgramIdentifierName().IsEqual(InProgramIdentifier))
		{
			OutProgram = It;
			return true;
		}
	}

	return false;
}

const bool UYetiOS_Core::IsProcessIdValid(const int32& InProcessID, class UYetiOS_BaseProgram*& OutProgram)
{
	OutProgram = RunningPrograms.FindRef(InProcessID);
	return (OutProgram != nullptr);
}

int32 UYetiOS_Core::AddRunningProgram(const class UYetiOS_BaseProgram* InNewProgram, FYetiOsError& OutErrorMessage)
{
	if (InNewProgram == nullptr)
	{
		static const FText Title = LOCTEXT("YetiOS_RunProgramError", "No program to add.");
		static const FText Description = LOCTEXT("YetiOS_RunProgramErrorDescription", "Cannot add a null program to list of running programs.");
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, RUN_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = RUN_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		return INDEX_NONE;
	}

	const FText MyProgramName = InNewProgram->GetProgramName();
	if (Device->IsInBsodState())
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_RunProgramBsodError", "Cannot change {0} state."), MyProgramName);
		static const FText Description = LOCTEXT("YetiOS_RunProgramBsodErrorDescription", "Cannot run program. System unstable.");
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, RUN_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = RUN_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		printlog_error(Description.ToString());
		return INDEX_NONE;
	}

	if (InNewProgram->IsSingleInstanceProgram() && IsProgramRunning(InNewProgram))
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_RunProgramInstanceError", "{0} already running."), MyProgramName);
		const FText Description = FText::Format(LOCTEXT("YetiOS_RunProgramInstanceErrorDescription", "{0} is already running and {0} does not support running more than one instance."), MyProgramName);		
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, RUN_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = RUN_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		return INDEX_NONE;
	}

	if (InNewProgram->IsCompatibleWithOS(this) == false)
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_RunProgramVersionError", "{0} incompatible."), MyProgramName);
		const FText Description = FText::Format(LOCTEXT("YetiOS_RunProgramVersionErrorDescription", "{0} is not compatible with this version of Operating System."), MyProgramName);
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, RUN_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = RUN_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		return INDEX_NONE;
	}

	int32 NewProcessID = FMath::RandRange(MIN_PROCES_ID_TO_GENERATE, MAX_PROCES_ID_TO_GENERATE);
	TArray<int32> ProcessIDs;
	RunningPrograms.GenerateKeyArray(ProcessIDs);
	while (ProcessIDs.Contains(NewProcessID))
	{
		NewProcessID = FMath::RandRange(MIN_PROCES_ID_TO_GENERATE, MAX_PROCES_ID_TO_GENERATE);
	}

	RunningPrograms.Add(NewProcessID, const_cast<UYetiOS_BaseProgram*>(InNewProgram));
	// #TERMINALPLUGIN TODO Consume resources after adding program.
	return NewProcessID;
}

void UYetiOS_Core::CloseRunningProgram(class UYetiOS_BaseProgram* InProgram, FYetiOsError& OutErrorMessage)
{
	if (InProgram == nullptr)
	{
		static const FText Title = LOCTEXT("YetiOS_RunProgramError", "No program to close.");
		static const FText Description = LOCTEXT("YetiOS_RunProgramErrorDescription", "Cannot close a null program.");
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, CLOSE_ERROR_CODE);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = CLOSE_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		printlog_error(Description.ToString());

		return;
	}

	const FText MyProgramName = InProgram->GetProgramName();
	if (Device->IsInBsodState())
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_CloseProgramBsodError", "Cannot change {0} state."), MyProgramName);
		static const FText Description = LOCTEXT("YetiOS_CloseProgramBsodErrorDescription", "Cannot close program. System unstable.");
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, CLOSE_ERROR_CODE, EYetiOsNotificationType::TYPE_Error);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = CLOSE_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		printlog_error(Description.ToString());

		return;
	}

	const int32 Local_ProcessID = InProgram->GetProcessID();
	if (RunningPrograms.Contains(Local_ProcessID) == false)
	{
		const FText Title = FText::Format(LOCTEXT("YetiOS_CloseProgramBsodError", "Cannot change {0} state."), MyProgramName);
		static const FText Description = LOCTEXT("YetiOS_CloseProgramBsodErrorDescription", "Cannot close program that is not running.");		
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, CLOSE_ERROR_CODE, EYetiOsNotificationType::TYPE_Warning);
		CreateOsNotification(NewNotification);

		OutErrorMessage.ErrorCode = CLOSE_ERROR_CODE;
		OutErrorMessage.ErrorException = Title;
		OutErrorMessage.ErrorDetailedException = Description;

		printlog_error(Description.ToString());

		return;
	}

	RunningPrograms.Remove(Local_ProcessID);
	RunningPrograms.Compact();
	// #TERMINALPLUGIN TODO Free resources after closing program.
}

void UYetiOS_Core::SetActiveUser(FYetiOsUser InNewUser)
{
	CurrentActiveUser = InNewUser;
}

bool UYetiOS_Core::ChangePassword(FYetiOsUser& InNewUser, FText InNewPassword)
{
	if (InNewUser.Password.EqualToCaseIgnored(InNewPassword) == false)
	{
		for (FYetiOsUser& It : OsUsers)
		{
			if (It == InNewUser)
			{
				InNewUser.Password = InNewPassword;
				It.Password = InNewPassword;

				const FText Title = FText::Format(LOCTEXT("YetiOS_UserPasswordChange", "{0} Password Change."), It.UserName);
				static const FText Description = LOCTEXT("YetiOS_UserPasswordChangeDescription", "Password has been updated for this user.");
				const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_OS, Title, Description, EYetiOsNotificationType::TYPE_Info);
				CreateOsNotification(NewNotification);

				return true;
			}
		}
	}

	return false;
}

void UYetiOS_Core::DestroyOS()
{
	FYetiOsNotificationManager::Destroy(NotificationManager);
	NotificationManager = nullptr;
	Device = nullptr;
	OsWidget = nullptr;
	AllCreatedDirectories.Empty();
	if (RootDirectory && RootDirectory->IsValidLowLevel())
	{
		RootDirectory->DestroyDirectory();
		RootDirectory = nullptr;
	}	
	OsWorld = nullptr;
	printlog_veryverbose(FString::Printf(TEXT("Destroyed operating system '%s'"), *OsName.ToString()));
	ConditionalBeginDestroy();
}

const bool UYetiOS_Core::UpdateWindowZOrder(class UYetiOS_DraggableWindowWidget* InWindow)
{
	if (InWindow && IsModalDialogOpen() == false)
	{		
		CurrentZOrder++;
		UCanvasPanelSlot* Slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(InWindow);
		Slot->SetZOrder(CurrentZOrder);
		return true;
	}

	return false;
}

UYetiOS_CPU* UYetiOS_Core::GetMainCpu() const
{
	int32 DummyTotal;
	return Device->GetMotherboard()->GetCpu(DummyTotal);
}

const bool UYetiOS_Core::HasValidRootDirectoryClass() const
{
	return (Device && Device->GetRootDirectoryClass() != nullptr);
}

void UYetiOS_Core::Internal_FinishOperatingSystemInstallation()
{
	Internal_InstallStartupPrograms();

	if (bIsPreInstalled == false)
	{
		static const FText Title = LOCTEXT("YetiOS_FinishInstallation", "Finish Installation.");
		static const FText Description = LOCTEXT("YetiOS_FinishInstallationDescription", "Operating system installation finished on device.");
		static const FText Code = LOCTEXT("YetiOS_FinishInstallationCode", "INSTALL_FINISH");
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_Device, Title, Description, Code);
		CreateOsNotification(NewNotification);
		printlog(Description.ToString());
	}

	OsWidget->FinishOsInstallation();
	Device->OnFinishInstallingOperatingSystem();
}

TSubclassOf<class UYetiOS_BaseProgram> UYetiOS_Core::Internal_FindProgramFromPackage(const FName& InProgramIdentifier)
{
	TSubclassOf<class UYetiOS_BaseProgram> ProgramClassToReturn = nullptr;
	if (HasRepositoryLibrary() && InProgramIdentifier.IsNone() == false)
	{
		TSet<FYetiOS_RepoProgram> AllProgramsFromRepo = ProgramsRepository->GetProgramsFromRepository();

		printlog(FString::Printf(TEXT("Looking for %s from %i package(s) in repo."), *InProgramIdentifier.ToString(), AllProgramsFromRepo.Num()));
		if (AllProgramsFromRepo.Num() > 0)
		{			
			for (const auto& It : AllProgramsFromRepo)
			{
				if (It.ProgramClass)
				{
					UYetiOS_BaseProgram* MyLoadedProgram = NewObject<UYetiOS_BaseProgram>(this, It.ProgramClass);
					if (MyLoadedProgram->GetProgramIdentifierName().IsEqual(InProgramIdentifier))
					{
						printlog(FString::Printf(TEXT("Found %s from package repo."), *InProgramIdentifier.ToString()));
						ProgramClassToReturn = MyLoadedProgram->GetClass();
						MyLoadedProgram->ConditionalBeginDestroy();
						MyLoadedProgram = nullptr;
						break;
					}
				}
			}
		}
	}

	if (ProgramClassToReturn == nullptr)
	{
		printlog_warn(FString::Printf(TEXT("Package %s not found in repo."), *InProgramIdentifier.ToString()));
	}

	return ProgramClassToReturn;
}

void UYetiOS_Core::Internal_InstallStartupPrograms()
{
	UYetiOS_AppIconWidget* OutIconWidget = nullptr;
	TSet<FYetiOS_RepoProgram> AllProgramsFromRepo = ProgramsRepository->GetProgramsFromRepository();
	FYetiOsError OutError;
	for (const auto& It : AllProgramsFromRepo)
	{
		if (It.bInstallWithOS)
		{
			UYetiOS_BaseProgram* Local_InstalledProgram = InstallProgram(It.ProgramClass, OutError, OutIconWidget);
			if (Local_InstalledProgram == nullptr)
			{
				printlog_warn(OutError.ErrorDetailedException.ToString());
			}
		}
	}
}

void UYetiOS_Core::OnOperatingSystemLoadedFromSaveGame(const class UYetiOS_SaveGame*& LoadGameInstance, FYetiOsError& OutErrorMessage)
{
	if (LoadGameInstance)
	{
		printlog_veryverbose("Loading OS save data...");
		GetOwningDevice()->GetMotherboard()->GetHardDisk()->ConsumeSpace(InstallationSpaceInMB);
		OsVersion = LoadGameInstance->GetOsLoadData().SaveLoad_OSVersion;
		OsUsers = LoadGameInstance->GetOsLoadData().SaveLoad_OsUsers;
		if (GetRootDirectory())
		{
			TArray<FYetiOsDirectorySaveLoad> SavedDirectories = LoadGameInstance->GetDirectoriesData();
			printlog_veryverbose(FString::Printf(TEXT("Loading %i saved directories..."), SavedDirectories.Num()));
			for (const FYetiOsDirectorySaveLoad& It : SavedDirectories)
			{
				UYetiOS_DirectoryBase* LoadedDirectory = CreateDirectoryInPath(It.SaveLoad_DirPath, It.bSaveLoad_IsHidden, OutErrorMessage, It.SaveLoad_DirectoryName);
				if (LoadedDirectory && It.SaveLoad_Files.Num() > 0)
				{
					printlog_veryverbose(FString::Printf(TEXT("Loading %i file(s) from save data for %s..."), It.SaveLoad_Files.Num(), *LoadedDirectory->GetDirectoryName().ToString()));
					for (const auto& LoadFileIt : It.SaveLoad_Files)
					{
						UYetiOS_FileBase* OutFile;
						LoadedDirectory->CreateNewFileByClass(LoadFileIt->GetClass(), OutFile, OutErrorMessage);
					}
				}
			}
		}

		Internal_InstallStartupPrograms();
		TArray<FYetiOsProgramSaveLoad> SavedPrograms = LoadGameInstance->GetProgramData();
		printlog_veryverbose(FString::Printf(TEXT("Loading %i saved programs..."), SavedPrograms.Num()));
		for (const auto& It : SavedPrograms)
		{
			UYetiOS_AppIconWidget* OutWidget;
			InstallProgram(It.SaveLoad_ProgramClass, OutErrorMessage, OutWidget);
		}

		GetOwningDevice()->GetMotherboard()->GetHardDisk()->Internal_UpdateRemainingSpace(LoadGameInstance->GetDeviceLoadData().SaveLoad_RemainingSpace);
	}
}

void UYetiOS_Core::NotifyBatteryLevelChange(const float& CurrentBatteryLevel)
{
	K2_OnBatteryLevelChanged(CurrentBatteryLevel);
	if (OsWidget)
	{
		OsWidget->OnBatteryLevelChanged(CurrentBatteryLevel);
	}
}

void UYetiOS_Core::NotifyLowBattery(const bool bIsLowBattery)
{
	if (OsWidget)
	{
		OsWidget->OnLowBatteryWarningReceived(bIsLowBattery);
	}
}

const bool UYetiOS_Core::IsProgramInstalled(const FName& InProgramIdentifier, UYetiOS_BaseProgram*& OutFoundProgram, FYetiOsError& OutErrorMessage) const
{
	OutFoundProgram = nullptr;
	
	if (InProgramIdentifier.IsNone() == false)
	{
		for (const auto& It : InstalledPrograms)
		{
			if (It->GetProgramIdentifierName().IsEqual(InProgramIdentifier))
			{
				OutFoundProgram = It;
				return true;
			}
		}

		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_NoProgramInstalledError", "PROGRAM_NOT_FOUND");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_NoProgramInstalledException", "Program {0} not installed."), FText::FromName(InProgramIdentifier));
	}
	else
	{
		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_EmptyProgramIdentifier", "ERR_EMPTY_PROGRAM_IDENTIFIER");
		OutErrorMessage.ErrorException = LOCTEXT("YetiOS_EmptyProgramIdentifierException", "Cannot check for an empty program.");
	}

	return false;
}

bool UYetiOS_Core::IsProgramInstalled(const FName& InProgramIdentifier) const
{
	UYetiOS_BaseProgram* DummyProgram;
	FYetiOsError DummyError;
	return IsProgramInstalled(InProgramIdentifier, DummyProgram, DummyError);
}

void UYetiOS_Core::AddToCreatedDirectories(const UYetiOS_DirectoryBase* InDirectory)
{
	AllCreatedDirectories.AddUnique(InDirectory);
}

bool UYetiOS_Core::HasRepositoryLibrary() const
{
	return ProgramsRepository != nullptr && ProgramsRepository->GetProgramsFromRepository().Num() > 0;
}

UYetiOS_DirectoryRoot* UYetiOS_Core::GetRootDirectory()
{
	if (RootDirectory == nullptr && ensureMsgf(HasValidRootDirectoryClass(), TEXT("Valid root directory check has failed. This should never happen. Make sure you have a valid device and it has a RootDirectoryClass specified.")))
	{
		RootDirectory = NewObject<UYetiOS_DirectoryRoot>(this, Device->GetRootDirectoryClass());
		AddToCreatedDirectories(RootDirectory);
		FYetiOsError OutError;
		RootDirectory->CreateNativeChildDirectories(this, OutError, true);
	}

	return RootDirectory;
}

const bool UYetiOS_Core::DirectoryExists(const FString& InDirectoryPath, UYetiOS_DirectoryBase*& OutFoundDirectory) const
{
	if (RootDirectory == nullptr)
	{
		return false;
	}

	if (RootDirectory->GetDirectoryName().ToLower().ToString() == InDirectoryPath.ToLower())
	{
		return true;
	}

	TArray<FString> OutPathArray;
	const FString DirectoryString = InDirectoryPath;
	DirectoryString.ParseIntoArray(OutPathArray, *UYetiOS_Core::PATH_DELIMITER, true);
	const int32 ArrayLength = OutPathArray.Num() - 1;

	UYetiOS_DirectoryBase* MyCurrentDirectory = RootDirectory;
	for (int32 i = 0; i < ArrayLength; ++i)
	{
		MyCurrentDirectory = MyCurrentDirectory->GetChildDirectoryByName(FName(*OutPathArray[i]), false);
		if (MyCurrentDirectory == nullptr)
		{
			return false;
		}
	}
	
	return MyCurrentDirectory->HasChildDirectory(FText::FromString(OutPathArray.Last()), OutFoundDirectory);
}

const bool UYetiOS_Core::DirectoryExists(const FString& InDirectoryPath) const
{
	UYetiOS_DirectoryBase* OutDirectory;
	return DirectoryExists(InDirectoryPath, OutDirectory);
}

UYetiOS_DirectoryBase* UYetiOS_Core::CreateDirectoryInPath(const FString& InDirectoryPath, const bool bHidden, FYetiOsError& OutErrorMessage, const FText& InDirName /*= FText::GetEmpty()*/)
{
	TArray<FString> OutPathArray;
	const FString DirectoryString = InDirectoryPath;
	DirectoryString.ParseIntoArray(OutPathArray, *UYetiOS_Core::PATH_DELIMITER, true);
	const int32 ArrayLength = OutPathArray.Num();

	UYetiOS_DirectoryBase* MyCurrentDirectory = RootDirectory;
	for (int32 i = 0; i < ArrayLength; ++i)
	{		
		UYetiOS_DirectoryBase* ExistingDir = MyCurrentDirectory->GetChildDirectoryByName(FName(*OutPathArray[i]), false);
		if (ExistingDir == nullptr)
		{			
			ExistingDir = MyCurrentDirectory->CreateChildDirectory(this, TemplateDirectory, OutErrorMessage, InDirName, bHidden, true);
			if (ExistingDir)
			{
				FString LeftS, RightS;
				DirectoryString.Split(OutPathArray[i], &LeftS, &RightS);
				printlog(FString::Printf(TEXT("Created child directory in %s"), *LeftS));
				return ExistingDir;
			}
		}
		else
		{
			MyCurrentDirectory = ExistingDir;
		}
	}

	printlog_error(FString::Printf(TEXT("Failed to create directory from path %s"), *DirectoryString));
	return nullptr;
}

TArray<class UYetiOS_BaseProgram*> UYetiOS_Core::GetRunningPrograms() const
{
	TArray<UYetiOS_BaseProgram*> OutArray;
	RunningPrograms.GenerateValueArray(OutArray);
	return OutArray;
}

class UYetiOS_BaseProgram* UYetiOS_Core::GetRunningProgramByIdentifier(const FName& InIdentifier) const
{
	TArray<UYetiOS_BaseProgram*> OutArray;
	RunningPrograms.GenerateValueArray(OutArray);
	UYetiOS_BaseProgram** FoundDevice = nullptr;
	FoundDevice = OutArray.FindByPredicate([InIdentifier](const UYetiOS_BaseProgram* InProgram)
	{
		return InProgram->GetProgramIdentifierName().IsEqual(InIdentifier);
	});

	if (FoundDevice == nullptr)
	{
		return nullptr;
	}

	return *FoundDevice;
}

bool UYetiOS_Core::GetTaskbar(UYetiOS_Taskbar*& OutTaskbar) const
{
	OutTaskbar = Taskbar;
	return OutTaskbar != nullptr;
}

bool UYetiOS_Core::GetStartMenu(UYetiOS_StartMenu*& OutStartMenu) const
{
	UYetiOS_Taskbar* OutTaskbar;
	if (GetTaskbar(OutTaskbar))
	{
		return OutTaskbar->GetStartMenu(OutStartMenu);
	}

	return false;
}

bool UYetiOS_Core::GetDesktopDirectory(UYetiOS_DirectoryBase*& OutDesktopDir) const
{
	const bool bIsValid = DesktopDirectory.IsValid();
	OutDesktopDir = bIsValid ? DesktopDirectory.Get() : nullptr;
	return bIsValid;
}

bool UYetiOS_Core::IsModalDialogOpen() const
{
	bool bModalDialogOpen = false;
	for (const auto& It : CurrentDialogWidgets)
	{
		if (It->IsModalDialog())
		{
			bModalDialogOpen = true;
			break;
		}
	}

	return bModalDialogOpen;
}

const TArray<class UYetiOS_BaseProgram*> UYetiOS_Core::GetInstalledPrograms(const bool bSystemProgramsOnly /*= false*/) const
{
	if (bSystemProgramsOnly)
	{
		TArray<UYetiOS_BaseProgram*> ReturnResult;
		for (const auto& It : InstalledPrograms)
		{
			if (It->IsSystemInstalledProgram())
			{
				ReturnResult.Add(It);
			}
		}

		return ReturnResult;
	}
	
	return InstalledPrograms;
}

const bool UYetiOS_Core::GetAllProgramsFromRepositoryLibrary(TArray<TSubclassOf<class UYetiOS_BaseProgram>>& OutPrograms)
{
	if (HasRepositoryLibrary())
	{
		TSet<FYetiOS_RepoProgram> AllProgramsFromRepo = ProgramsRepository->GetProgramsFromRepository();

		for (const auto& It : AllProgramsFromRepo)
		{
			UYetiOS_BaseProgram* MyLoadedProgram = NewObject<UYetiOS_BaseProgram>(this, It.ProgramClass);
			OutPrograms.Add(MyLoadedProgram->GetClass());
		}

		return true;
	}

	OutPrograms = TArray<TSubclassOf<class UYetiOS_BaseProgram>>();
	return false;
}

#undef printlog
#undef printlog_warn
#undef printlog_error
#undef printlog_veryverbose

#undef LOCTEXT_NAMESPACE
