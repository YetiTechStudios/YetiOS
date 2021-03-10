// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_BaseProgram.h"
#include "Core/YetiOS_Core.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Widgets/YetiOS_AppWidget.h"
#include "Widgets/YetiOS_OsWidget.h"
#include "Misc/YetiOS_ProgramSettings.h"
#include "Core/YetiOS_FileBase.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "Widgets/YetiOS_DraggableWindowWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsBaseProgram, All, All)

#define printlog(Param1)				UE_LOG(LogYetiOsBaseProgram, Log, TEXT("%s"), *FString(Param1))
#define printlog_error(Param1)			UE_LOG(LogYetiOsBaseProgram, Error, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_BaseProgram::UYetiOS_BaseProgram()
{
	ProgramName = FText::GetEmpty();
	ProgramIcon = nullptr;	

	SettingsClass = nullptr;
	SaveMethod = EProgramSaveMethod::SaveOnExit;

	bSingleInstanceOnly = false;
	bCanRunOnPortableDevice = true;
	bIsSystemInstalledProgram = false;
	bAddToDesktop = true;
	bCreateInstallationNotification = true;
	ProgramIconWidget = nullptr;

	bSupportStore = true;
	bCanCallOnCreate = true;
	bCanCallOnStart = true;
	bCanCallOnClose = true;
	bCanCallOnInstall = true;

	bOverrideWindowSize = false;
	OverrideWindowSize = FVector2D(640.f, 480.f);

	ProcessID = INDEX_NONE;

	CurrentVisibilityState = EYetiOsProgramVisibilityState::STATE_Normal;
}

UYetiOS_BaseProgram* UYetiOS_BaseProgram::CreateProgram(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_BaseProgram> ProgramClass, FYetiOsError& OutErrorMessage, const bool bInstalledWithOS /*= false*/)
{
	if (InOS && !InOS->IsPendingKill())
	{
		UYetiOS_BaseProgram* ProxyProgram = NewObject<UYetiOS_BaseProgram>(InOS, ProgramClass);
		const UYetiOS_BaseDevice* OwningDevice = InOS->GetOwningDevice();
		if (OwningDevice->IsPortableDevice() && ProxyProgram->bCanRunOnPortableDevice == false)
		{
			printlog_error(FString::Printf(TEXT("%s cannot run on %s portable device."), *ProxyProgram->ProgramName.ToString(), *OwningDevice->GetDeviceName().ToString()));
			OutErrorMessage.ErrorCode = FText::AsCultureInvariant("ERR_INCOMPATIBLE_DEVICE");
			OutErrorMessage.ErrorException = LOCTEXT("YetiOS_InCompatibleDeviceErrorException", "Incompatible device.");
			OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_InCompatibleDeviceErrorDetailedException", "{0} is not compatible with this device."), ProxyProgram->ProgramName);
			const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, OutErrorMessage.ErrorException, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode, EYetiOsNotificationType::TYPE_Error);
			InOS->CreateOsNotification(NewNotification);
			ProxyProgram->ConditionalBeginDestroy();
			ProxyProgram = nullptr;
		}
		else
		{
			const bool bIsCompatible = ProxyProgram->IsCompatibleWithOS(InOS);
			if (bIsCompatible == false)
			{
				const FText Title = FText::Format(LOCTEXT("YetiOS_RunProgramVersionError", "{0} incompatible."), ProxyProgram->ProgramName);
				const FText Description = FText::Format(LOCTEXT("YetiOS_RunProgramVersionErrorDescription", "{0} is not compatible with this version of Operating System."), ProxyProgram->ProgramName);
				const FText RunError = LOCTEXT("YetiOS_RunProgramVersionErrorCode", "ERR_RUN_PROGRAM");
				const FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, Title, Description, RunError, EYetiOsNotificationType::TYPE_Error);
				InOS->CreateOsNotification(NewNotification);
				OutErrorMessage = GetErrorStruct(RunError, Title, Description);
				printlog_error(FString::Printf(TEXT("%s not created. Incorrect version."), *ProxyProgram->ProgramName.ToString()));
				ProxyProgram->ConditionalBeginDestroy();
				ProxyProgram = nullptr;
			}
			else
			{
				ProxyProgram->OwningOS = InOS;
				ProxyProgram->bIsSystemInstalledProgram = bInstalledWithOS;
				if (ProxyProgram->bCreateSettingsOnProgramCreation)
				{
					ProxyProgram->Internal_LoadProgramSettings();
				}

				if (ProxyProgram->bCanCallOnCreate)
				{
					ProxyProgram->K2_OnCreate();
				}

				printlog(FString::Printf(TEXT("%s created."), *ProxyProgram->ProgramName.ToString()));
			}
		}

		
		return ProxyProgram;
	}

	return nullptr;
}

UYetiOS_BaseProgram* UYetiOS_BaseProgram::Internal_StartProgram(FYetiOsError& OutErrorMessage)
{
	if (IsRunning() && IsSingleInstanceProgram())
	{
		return this;
	}

	{
		{
		}
	}

	UYetiOS_BaseProgram* ProxyProgram = CreateProgram(OwningOS, GetClass(), OutErrorMessage);
	if (ProxyProgram)
	{
		const int32 MyProcessID = ProxyProgram->OwningOS->AddRunningProgram(ProxyProgram, OutErrorMessage);
		if (MyProcessID != INDEX_NONE)
		{
			ProxyProgram->bIsSystemInstalledProgram = bIsSystemInstalledProgram;
			ProxyProgram->ProgramWidget = UYetiOS_AppWidget::Internal_CreateAppWidget(ProxyProgram);
			ProxyProgram->ProcessID = MyProcessID;
			ProxyProgram->OwningWindow = ProxyProgram->OwningOS->GetOsWidget()->CreateNewWindow(ProxyProgram, ProxyProgram->ProgramWidget, ProxyProgram->bOverrideWindowSize ? ProxyProgram->OverrideWindowSize : FVector2D::ZeroVector);
			printlog(FString::Printf(TEXT("Executing program %s..."), *ProxyProgram->ProgramName.ToString()));
			if (ProxyProgram->bCanCallOnStart)
			{
				ProxyProgram->K2_OnStart();
			}

			ProxyProgram->Internal_LoadProgramSettings();
		}
		else
		{
			ProxyProgram->ConditionalBeginDestroy();
			ProxyProgram = nullptr;
		}
	}

	return ProxyProgram;
}

void UYetiOS_BaseProgram::ProgramInstalled()
{
	if (bCanCallOnInstall)
	{
		K2_OnInstall();
	}
}

const bool UYetiOS_BaseProgram::StartProgram()
{
	UYetiOS_BaseProgram* OutProgram;
	FYetiOsError OutError;
	return StartProgram(OutProgram, OutError);
}

const bool UYetiOS_BaseProgram::StartProgram(FYetiOsError& OutErrorMessage)
{
	UYetiOS_BaseProgram* OutProgram;
	return StartProgram(OutProgram, OutErrorMessage);
}

const bool UYetiOS_BaseProgram::StartProgram(UYetiOS_BaseProgram*& OutProgram, FYetiOsError& OutErrorMessage)
{	
	OutProgram = Internal_StartProgram(OutErrorMessage);
	return OutProgram != nullptr;
}

bool UYetiOS_BaseProgram::ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState)
{
	if (CurrentVisibilityState != InNewState)
	{
		CurrentVisibilityState = InNewState;
		if (ProgramWidget)
		{
			ProgramWidget->Internal_OnChangeVisibilityState(CurrentVisibilityState);
		}
		return true;
	}

	return false;
}

void UYetiOS_BaseProgram::AddProgramIconWidget(class UYetiOS_AppIconWidget* InIconWidget)
{
	ProgramIconWidget = InIconWidget;
}

bool UYetiOS_BaseProgram::OpenFile(class UYetiOS_FileBase* InFileToOpen, const bool bInForceOpenInSameInstance)
{
	bool bLocal_ShouldStartProgram = true;

	if (InFileToOpen->CanOpenInSameInstance())
	{
		bLocal_ShouldStartProgram = IsRunning() == false;
	}
	else
	{
		if (bInForceOpenInSameInstance)
		{
			bLocal_ShouldStartProgram = false;
		}
	}

	if (bLocal_ShouldStartProgram)
	{
		UYetiOS_BaseProgram* OutProgram;
		FYetiOsError OutError;
		if (StartProgram(OutProgram, OutError))
		{
			// We need to create a temp file as new to avoid issues since the same UObject or its child maybe referenced elsewhere.			
			UYetiOS_FileBase* TempFileCreated = NewObject<UYetiOS_FileBase>(InFileToOpen->GetParentDirectory(), InFileToOpen->GetClass());
			OutProgram->CurrentFileOpened = TempFileCreated;
			OutProgram->ProgramWidget->SetFileWidget(OutProgram->CurrentFileOpened);
			OutProgram->K2_OnOpenFile();
			return true;
		}

		return false;
	}

	CurrentFileOpened = InFileToOpen;
	ProgramWidget->SetFileWidget(CurrentFileOpened);
	K2_OnOpenFile();
	OwningWindow->BringWindowToFront();
	return true;
}

void UYetiOS_BaseProgram::CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown /*= false*/)
{
	if (bCanCallOnClose)
	{
		K2_OnClose();
	}

	OwningOS->CloseRunningProgram(this, OutErrorMessage);
	ProcessID = INDEX_NONE;

	if (CurrentFileOpened)
	{
		CurrentFileOpened->CloseFile();
		CurrentFileOpened->ConditionalBeginDestroy();
		CurrentFileOpened = nullptr;
	}

	if (bIsOperatingSystemShuttingDown)
	{
		if (SaveMethod == EProgramSaveMethod::SaveOnOperatingSystemShutdown || SaveMethod == EProgramSaveMethod::SaveOnExit)
		{
			UYetiOS_ProgramSettings::SaveProgramSettings(this);
		}
	}
	else if (SaveMethod == EProgramSaveMethod::SaveOnExit)
	{
		UYetiOS_ProgramSettings::SaveProgramSettings(this);
	}

	if (ProgramWidget)
	{
		ProgramWidget->SetFileWidget(nullptr);
		ProgramWidget->DestroyProgramWidget();
		ProgramWidget = nullptr;
	}

	if (bIsOperatingSystemShuttingDown && ProgramSettings)
	{
		ProgramSettings->Destroy();
		ProgramSettings = nullptr;
	}

	OwningWindow->CloseWindow();
	OwningWindow = nullptr;

	printlog(FString::Printf(TEXT("Program %s closed."), *ProgramName.ToString()));
	ConditionalBeginDestroy();
}

bool UYetiOS_BaseProgram::Internal_LoadProgramSettings()
{
	if (ProgramSettings)
	{
		ProgramSettings->ConditionalBeginDestroy();
		ProgramSettings = nullptr;
	}

	// First try to set from a load class
	ProgramSettings = UYetiOS_ProgramSettings::LoadSettings(this);
	if (ProgramSettings)
	{
		printlog(FString::Printf(TEXT("Loading saved settings for %s."), *ProgramName.ToString()));
		K2_OnSettingsLoad();
		return true;
	}

	// Program settings was invalid. So create and assign one.
	if (SettingsClass)
	{
		ProgramSettings = UYetiOS_ProgramSettings::CreateSettings(this, SettingsClass);
		printlog(FString::Printf(TEXT("Created new settings class for %s."), *ProgramName.ToString()));
		return true;
	}

	return false;
}

bool UYetiOS_BaseProgram::IsCompatibleWithOS(class UYetiOS_Core* InOS) const
{
	if (bRequireMinimumOperatingSystemVersion)
	{
		return UYetiOS_Core::IsVersionsCompatible(MinimumOperatingSystemVersionRequired, InOS->GetOsVersion());
	}

	return true;
}

#undef printlog
#undef printlog_error

#undef LOCTEXT_NAMESPACE
