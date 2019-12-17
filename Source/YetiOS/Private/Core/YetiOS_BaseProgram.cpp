// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_BaseProgram.h"
#include "Core/YetiOS_Core.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Widgets/YetiOS_AppWidget.h"
#include "Widgets/YetiOS_OsWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsBaseProgram, All, All)

#define printlog_display(Param1)		UE_LOG(LogYetiOsBaseProgram, Display, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_BaseProgram::UYetiOS_BaseProgram()
{
	ProgramName = FText::GetEmpty();
	ProgramIcon = nullptr;	
	bSingleInstanceOnly = true;
	bIsSystemInstalledProgram = false;

	bCanCallOnCreate = true;
	bCanCallOnStart = true;
	bCanCallOnClose = true;

	CurrentVisibilityState = EYetiOsProgramVisibilityState::STATE_Normal;
}

UYetiOS_BaseProgram* UYetiOS_BaseProgram::CreateProgram(UYetiOS_Core* InOS, TSubclassOf<UYetiOS_BaseProgram> ProgramClass, FYetiOsError& OutErrorMessage, const bool bStartImmediately /*= false*/, const bool bIsOsPredefinedProgram /*= false*/)
{
	if (InOS && !InOS->IsPendingKill())
	{
		UYetiOS_BaseProgram* ProxyProgram = NewObject<UYetiOS_BaseProgram>(InOS, ProgramClass);
		ProxyProgram->OwningOS = InOS;
		ProxyProgram->bIsSystemInstalledProgram = bIsOsPredefinedProgram;
		
		if (ProxyProgram->bCanCallOnCreate)
		{
			ProxyProgram->K2_OnCreate();
		}

		printlog_display(FString::Printf(TEXT("%s created."), *ProxyProgram->ProgramName.ToString()));
		if (bStartImmediately)
		{
			ProxyProgram->StartProgram(OutErrorMessage);
		}

		return ProxyProgram;
	}

	return nullptr;
}

const bool UYetiOS_BaseProgram::StartProgram(FYetiOsError& OutErrorMessage)
{
	const int32 MyProcessID = OwningOS->AddRunningProgram(this, OutErrorMessage);
	if (MyProcessID != INDEX_NONE)
	{
		ProgramWidget = UYetiOS_AppWidget::Internal_CreateAppWidget(this);
		ProcessID = MyProcessID;
		OwningOS->GetOsWidget()->K2_CreateNewWindow(this, ProgramWidget);
		printlog_display(FString::Printf(TEXT("Executing program %s..."), *ProgramName.ToString()));
		if (bCanCallOnStart)
		{
			K2_OnStart();
		}
		return true;
	}

	return false;
}

void UYetiOS_BaseProgram::CloseProgram(FYetiOsError& OutErrorMessage)
{
	if (bCanCallOnClose)
	{
		K2_OnClose();
	}
	OwningOS->CloseRunningProgram(this, OutErrorMessage);
	ProcessID = INDEX_NONE;
	ProgramWidget->DestroyProgramWidget();
	ProgramWidget = nullptr;
	printlog_display(FString::Printf(TEXT("Program %s closed."), *ProgramName.ToString()));
}

bool UYetiOS_BaseProgram::ChangeVisibilityState(const EYetiOsProgramVisibilityState InNewState)
{
	if (CurrentVisibilityState != InNewState)
	{
		CurrentVisibilityState = InNewState;
		ProgramWidget->Internal_OnChangeVisibilityState(CurrentVisibilityState);
		return true;
	}

	return false;
}

#undef printlog_display
#undef LOCTEXT_NAMESPACE
