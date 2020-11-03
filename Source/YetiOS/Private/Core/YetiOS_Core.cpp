// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Core/YetiOS_DirectoryRoot.h"
#include "Core/YetiOS_SaveGame.h"
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


DEFINE_LOG_CATEGORY_STATIC(LogYetiOsOperatingSystem, All, All)

#define printlog_display(Param1)		UE_LOG(LogYetiOsOperatingSystem, Display, TEXT("%s"), *FString(Param1))
#define printlog_warn(Param1)			UE_LOG(LogYetiOsOperatingSystem, Warning, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsOperatingSystem, Error, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsOperatingSystem, VeryVerbose, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

static const int32 MIN_PROCES_ID_TO_GENERATE = 1;
static const int32 MAX_PROCES_ID_TO_GENERATE = 99999;

const FString UYetiOS_Core::PATH_DELIMITER = "/";
const FText UYetiOS_Core::ROOT_USER_NAME = FText::AsCultureInvariant("root");

static const FText INSTALL_ERROR_CODE = LOCTEXT("YetiOS_InstallProgramErrorCode", "ERR_INSTALL_FAIL");
static const FText CLOSE_ERROR_CODE = LOCTEXT("YetiOS_CloseProgramErrorCode", "ERR_CLOSE_PROGRAM");
static const FText RUN_ERROR_CODE = LOCTEXT("YetiOS_RunProgramInstanceErrorCode", "ERR_RUN_PROGRAM");

UYetiOS_Core::UYetiOS_Core()
{
	OsName = FText::GetEmpty();
	OsVersion = FText::AsCultureInvariant("1.0.0.0");
	OsIcon = nullptr;
	InstallationSpace = 10.f;
	ReleaseState = EYetiOsOperatingSystemReleaseState::STATE_FullRelease;
	MinInstallationTime = 10.f;
	MaxInstallationTime = 60.f;

	CurrentZOrder = INDEX_NONE;

	RootUser = FYetiOsUser(UYetiOS_Core::ROOT_USER_NAME.ToString());
	RootCommand = FText::AsCultureInvariant("sudo");
}

UYetiOS_Core* UYetiOS_Core::CreateOperatingSystem(class UYetiOS_BaseDevice* InParentDevice, FYetiOsError& OutErrorMessage)
{
	UYetiOS_Core* ProxyOS = NewObject<UYetiOS_Core>(InParentDevice, InParentDevice->OperatingSystemClass);
	if (ProxyOS->GetCompatibleDeviceClasses().Contains(InParentDevice->GetClass()))
	{
		ProxyOS->Device = InParentDevice;
		ProxyOS->OsWorld = InParentDevice->GetWorld();
		ProxyOS->OsWidget = UYetiOS_OsWidget::Internal_CreateOsWidget(ProxyOS);
		ProxyOS->NotificationManager = FYetiOsNotificationManager::CreateNotificationManager();
		ProxyOS->InstalledPrograms.Empty();
		ProxyOS->RemainingSpace = InParentDevice->GetHardDisk().HddCapacity;
		return ProxyOS;
	}

	OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_CreateOperatingSystemErrorCode", "INCOMPATIBLE_OPERATING_SYSTEM");
	OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_CreateOperatingSystemErrorException", "{0} is not compatible with this device."), ProxyOS->GetOsName());	
	ProxyOS->ConditionalBeginDestroy();
	return nullptr;
}

const FText UYetiOS_Core::GetTimeAsText(const FDateTime& InDateTime)
{
	return FText::AsTime(InDateTime, EDateTimeStyle::Short, FText::GetInvariantTimeZone());
}

void UYetiOS_Core::CreateOsNotification(const FYetiOsNotification InNewNotification)
{
	OsWidget->ReceiveNotification(InNewNotification);
	NotificationManager->LogNotification(InNewNotification);
}

const bool UYetiOS_Core::StartOperatingSystemInstallation(const bool bShowBsodIfInstallationFails, FYetiOsError& OutErrorMessage)
{
	if (Internal_ConsumeSpace(InstallationSpace))
	{
		static const FText Title = LOCTEXT("YetiOS_StartInstallation", "Begin Installation.");
		static const FText Description = LOCTEXT("YetiOS_StartInstallationDescription", "Operating system installation started on device.");
		static const FText Code = LOCTEXT("YetiOS_StartInstallationCode", "INSTALL_START");
		const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_Device, Title, Description, Code);
		CreateOsNotification(NewNotification);

		CalculatedInstallationTime = FMath::Lerp<float>(MaxInstallationTime, MinInstallationTime, Device->GetDeviceScore());

		printlog_display(FString::Printf(TEXT("%s Will finish in %f seconds."), *Description.ToString(), CalculatedInstallationTime));
		OsWidget->StartOsInstallation(CalculatedInstallationTime);

		OsWorld->GetTimerManager().SetTimer(TimerHandle_OsInstallation, this, &UYetiOS_Core::Internal_FinishOperatingSystemInstallation, CalculatedInstallationTime, false);
		return true;
	}

	static const FText Exception = LOCTEXT("YetiOS_StartInstallation", "Insufficient space.");
	static const FText DetailedException = LOCTEXT("YetiOS_StartInstallationDescription", "Not enough space to install Operating System on this device.");
	static const FText Code = LOCTEXT("YetiOS_StartInstallationCode", "OS_INSTALL_FAIL");
	OutErrorMessage.ErrorCode = Code;
	OutErrorMessage.ErrorException = Exception;
	OutErrorMessage.ErrorDetailedException = DetailedException;
	if (bShowBsodIfInstallationFails)
	{		
		AYetiOS_DeviceManagerActor::ShowBSOD(this, Device, Code, Exception, DetailedException);
	}

	return false;
}

void UYetiOS_Core::LoadOS()
{
	OsWidget->BeginLoadOS();
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
	if (InNewUser.UserName.EqualToCaseIgnored(UYetiOS_Core::ROOT_USER_NAME))
	{
		const FText Title = LOCTEXT("YetiOS_InvalidUsernameErrorText", "Restricted Username");
		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_InvalidUsernameError", "ERR_INVALID_USERNAME");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_InvalidUsernameErrorException", "You cannot use '{0}' as your username."), UYetiOS_Core::ROOT_USER_NAME);
		OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_InvalidUsernameErrorException", "'{0}' is a system reserved username. Please try again with another name."), UYetiOS_Core::ROOT_USER_NAME);
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

	if (DefaultConstructed->GetProgramSpace() > RemainingSpace)
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

	const bool bIsPredefinedProgram = ProgramsToInstall.Contains(InProgramToInstall);
	UYetiOS_BaseProgram* NewProgram = UYetiOS_BaseProgram::CreateProgram(this, InProgramToInstall, OutErrorMessage, false, bIsPredefinedProgram);
	if (NewProgram)
	{
		Internal_ConsumeSpace(NewProgram->GetProgramSpace());
		OutIconWidget = UYetiOS_AppIconWidget::CreateProgramIconWidget(this, NewProgram, OutErrorMessage);
		InstalledPrograms.Add(NewProgram);
		printlog_display(FString::Printf(TEXT("Program %s installed."), *NewProgram->GetProgramName().ToString()));
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

void UYetiOS_Core::UpdateWindowZOrder(class UYetiOS_DraggableWindowWidget* InWindow)
{
	if (InWindow)
	{
		CurrentZOrder++;
		UCanvasPanelSlot* Slot = UWidgetLayoutLibrary::SlotAsCanvasSlot(InWindow);
		Slot->SetZOrder(CurrentZOrder);
	}
}

const FYetiOsCpu UYetiOS_Core::GetMainCpu() const
{
	if (Device->IsPortableDevice())
	{
		return Device->GetCastedDevice<UYetiOS_PortableDevice>()->GetCpu();
	}

	return Device->GetCastedDevice<UYetiOS_StationaryDevice>()->GetCpu(0);
}

const float UYetiOS_Core::GetTotalMemory(const bool bInBytes /*= true*/) const
{
	const float TotalMemory = Device->GetTotalMemorySize();
	return bInBytes ? TotalMemory * 1000000.f : TotalMemory;
}

const float UYetiOS_Core::GetTotalCPUSpeed(const bool bWithDurability /*= true*/) const
{
	return Device->GetTotalCpuSpeed(bWithDurability);
}

const bool UYetiOS_Core::HasGpuInstalled() const
{
	return Device->IsGpuInstalled();
}

const float UYetiOS_Core::GetDeviceScore() const
{
	return Device->GetDeviceScore(true);
}

const bool UYetiOS_Core::HasValidRootDirectoryClass() const
{
	return (Device && Device->GetRootDirectoryClass() != nullptr);
}

void UYetiOS_Core::Internal_FinishOperatingSystemInstallation()
{
	static const FText Title = LOCTEXT("YetiOS_FinishInstallation", "Finish Installation.");
	static const FText Description = LOCTEXT("YetiOS_FinishInstallationDescription", "Operating system installation finished on device.");
	static const FText Code = LOCTEXT("YetiOS_FinishInstallationCode", "INSTALL_FINISH");
	const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_Device, Title, Description, Code);
	CreateOsNotification(NewNotification);

	printlog_display(Description.ToString());

	OsWidget->FinishOsInstallation();
	Device->OnFinishInstallingOperatingSystem();
}

TSubclassOf<class UYetiOS_BaseProgram> UYetiOS_Core::Internal_FindProgramFromPackage(const FName& InProgramIdentifier)
{
	if (HasRepositoryLibrary() && InProgramIdentifier.IsNone() == false)
	{
		TArray<UBlueprintGeneratedClass*> OutBlueprintGeneratedClass;
		RepositoryLibrary->GetObjects<UBlueprintGeneratedClass>(OutBlueprintGeneratedClass);

		printlog_display(FString::Printf(TEXT("Looking for %s from %i package(s) in repo."), *InProgramIdentifier.ToString(), OutBlueprintGeneratedClass.Num()));
		if (OutBlueprintGeneratedClass.Num() > 0)
		{			
			for (const auto& It : OutBlueprintGeneratedClass)
			{
				UYetiOS_BaseProgram* MyLoadedProgram = NewObject<UYetiOS_BaseProgram>(this, It);
				if (MyLoadedProgram->GetProgramIdentifierName().IsEqual(InProgramIdentifier))
				{
					printlog_display(FString::Printf(TEXT("Found %s from package repo."), *InProgramIdentifier.ToString()));
					return MyLoadedProgram->GetClass();
				}
			}
		}
	}

	printlog_warn(FString::Printf(TEXT("Package %s not found in repo."), *InProgramIdentifier.ToString()));
	return nullptr;
}

const bool UYetiOS_Core::Internal_ConsumeSpace(float InSpaceToConsume)
{
	if (InSpaceToConsume < RemainingSpace)
	{
		RemainingSpace -= InSpaceToConsume;
		return true;
	}

	return false;
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
	return RepositoryLibrary != nullptr && RepositoryLibrary->GetObjectCount() > 0;
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
				printlog_display(FString::Printf(TEXT("Created child directory in %s"), *LeftS));
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

const bool UYetiOS_Core::GetAllProgramsFromRepositoryLibrary(TArray<TSubclassOf<class UYetiOS_BaseProgram>>& OutPrograms)
{
	if (HasRepositoryLibrary())
	{
		TArray<UBlueprintGeneratedClass*> OutBlueprintGeneratedClass;
		RepositoryLibrary->GetObjects<UBlueprintGeneratedClass>(OutBlueprintGeneratedClass);

		if (OutBlueprintGeneratedClass.Num() > 0)
		{
			for (const auto& It : OutBlueprintGeneratedClass)
			{
				UYetiOS_BaseProgram* MyLoadedProgram = NewObject<UYetiOS_BaseProgram>(this, It);
				OutPrograms.Add(MyLoadedProgram->GetClass());
			}
		}

		return true;
	}

	OutPrograms = TArray<TSubclassOf<class UYetiOS_BaseProgram>>();
	return false;
}

#undef printlog_display
#undef printlog_warn
#undef printlog_error
#undef printlog_veryverbose

#undef LOCTEXT_NAMESPACE
