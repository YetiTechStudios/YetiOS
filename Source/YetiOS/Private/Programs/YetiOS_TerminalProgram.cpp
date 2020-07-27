// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Programs/YetiOS_TerminalProgram.h"
#include "Core/YetiOS_DirectoryRoot.h"
#include "Core/YetiOS_Core.h"
#include "Misc/YetiOS_TerminalCommand.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/BlueprintGeneratedClass.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsTerminalProgram, All, All)

#define printlog_veryverbose(Param1)				UE_LOG(LogYetiOsTerminalProgram, VeryVerbose, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_TerminalProgram::UYetiOS_TerminalProgram()
{
	TerminalName = FName("Yeti Terminal");
	DefaultStartDirectory = FName("/Home");
	CommandLibrary = nullptr;
	bSupportRunningMultiCommands = true;

	ActiveCommandObject = nullptr;
	CurrentDirectory = nullptr;
	CurrentDirectoryPath = "";

	bEnableCommandHistory = true;
	bAllowAllCommandsInHistory = true;
	CommandHistoryIndex = INDEX_NONE;
}

const FText UYetiOS_TerminalProgram::GetRootCommandName()
{
	return UYetiOS_Core::ROOT_COMMAND;
}

const bool UYetiOS_TerminalProgram::StartProgram(FYetiOsError& OutErrorMessage)
{
	ChangeCurrentPath(DefaultStartDirectory.ToString());

	printlog_veryverbose(FString::Printf(TEXT("Current directory path: %s"), *CurrentDirectoryPath));
	return Super::StartProgram(OutErrorMessage);
}

void UYetiOS_TerminalProgram::CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown /*= false*/)
{
	CommandHistory.Empty();
	if (IsTerminalBusy())
	{
		ActiveCommandObject->Internal_TerminateCommand();
		ActiveCommandObject = nullptr;
	}

	Super::CloseProgram(OutErrorMessage);
}

void UYetiOS_TerminalProgram::CheckUserPrompt(const bool bIsYes)
{
	bIsYes ? ActiveCommandObject->Internal_OnContinueExecution() : ActiveCommandObject->OnSuccess();
}

void UYetiOS_TerminalProgram::PromptForUserContinueInput()
{
	K2_OnPromptUserContinue();
}

const bool UYetiOS_TerminalProgram::CheckRootPassword(const FText& InPasswordToCheck, const bool bCaseSensitive /*= true*/)
{	
	if (OwningOS && OwningOS->GetCurrentUser().UserName.IsEmptyOrWhitespace() == false)
	{
		const bool bPasswordMatch = bCaseSensitive ? OwningOS->GetCurrentUser().Password.EqualTo(InPasswordToCheck) : OwningOS->GetCurrentUser().Password.EqualToCaseIgnored(InPasswordToCheck);
		if (bPasswordMatch)
		{
			ActiveCommandObject->Internal_ProcessCommandAsRoot();
			return true;
		}
	}

	return false;
}

const bool UYetiOS_TerminalProgram::ChangeCurrentPath(const FString& NewPath)
{
	if (CurrentDirectory == nullptr)
	{
		UYetiOS_DirectoryRoot* MyRootDirectory = OwningOS->GetRootDirectory();
		CurrentDirectory = MyRootDirectory;
	}
	
	UYetiOS_DirectoryBase* NewDirectory = CurrentDirectory->GetDirectoryFromPath(NewPath);
	if (NewDirectory)
	{
		CurrentDirectory = NewDirectory;
		CurrentDirectory->EnsureOS(OwningOS);
		CurrentDirectoryPath = CurrentDirectory->GetFullPath(false);
		return true;
	}

	return false;
}

const int32 UYetiOS_TerminalProgram::GetTotalCommands() const
{
	return CommandLibrary && CommandLibrary->GetObjectCount();
}

const bool UYetiOS_TerminalProgram::HasPackageRepo() const
{
	return OwningOS && OwningOS->HasRepositoryLibrary();
}

TArray<class UYetiOS_TerminalCommand*> UYetiOS_TerminalProgram::GetAllCommands() const
{	
	TArray<UBlueprintGeneratedClass*> OutArray;
	CommandLibrary->GetObjects<UBlueprintGeneratedClass>(OutArray);

	TArray<class UYetiOS_TerminalCommand*> ReturnResult;
	for (const auto& It : OutArray)
	{
		ReturnResult.Add(NewObject<UYetiOS_TerminalCommand>(const_cast<UYetiOS_TerminalProgram*>(this), It));
	}

	return ReturnResult;
}

const FString UYetiOS_TerminalProgram::GetEachCommandFromHistory(const bool bFromStart /*= true*/)
{
	if (CommandHistory.Num() > 0)
	{
		if (bFromStart)
		{
			CommandHistoryIndex++;
		}
		else
		{
			if (CommandHistoryIndex == INDEX_NONE)
			{
				CommandHistoryIndex = CommandHistory.Num() - 1;
			}
			else
			{
				CommandHistoryIndex--;
			}
		}

		if (CommandHistory.IsValidIndex(CommandHistoryIndex) == false)
		{
			CommandHistoryIndex = bFromStart ? 0 : CommandHistory.Num() - 1;
		}
			
		
		printlog_veryverbose(FString::Printf(TEXT("%s (%i)"), *CommandHistory[CommandHistoryIndex], CommandHistoryIndex));
		return CommandHistory[CommandHistoryIndex];
	}

	printlog_veryverbose(FString::Printf(TEXT("No command found. Empty array probably. (%i)"), CommandHistoryIndex));
	return FString("");	
}

void UYetiOS_TerminalProgram::ReceiveMessageFromCommand(const FText& InMessage, EYetiOsTerminalMessageLevel InMessageType)
{
	K2_OnMessageReceivedFromCommand(InMessage, InMessageType);
}

void UYetiOS_TerminalProgram::StopCommand(const bool bIsSuccess, const FText& InErrorMessage)
{
	ActiveCommandObject = nullptr;
	if (bIsSuccess && QueuedCommands.IsValidIndex(0))
	{
		const FString NextCommand = QueuedCommands[0];
		QueuedCommands.RemoveAt(0);
		ProcessCommand(NextCommand);
		return;
	}
	
	K2_OnCommandFinished(bIsSuccess, InErrorMessage);
}

void UYetiOS_TerminalProgram::ChangeCurrentUser(const FYetiOsUser& InNewUser)
{
	bIsRunningAsRoot = InNewUser == OwningOS->GetRootUser();
	CurrentUser = InNewUser;
	K2_OnUserChanged(bIsRunningAsRoot, CurrentUser);
}

void UYetiOS_TerminalProgram::PromptRootPassword()
{
	K2_OnPromptRootPassword();
}

const bool UYetiOS_TerminalProgram::ProcessCommand(FString InCommand)
{	
	if (IsTerminalBusy() == false)
	{
		const FString RawCommand = InCommand;
		ActiveCommandObject = Internal_GetTerminalCommandObject(RawCommand);
		if (ActiveCommandObject)
		{			
			if (bSupportRunningMultiCommands && InCommand.Contains(" && "))
			{
				QueuedCommands.Empty();
				InCommand.ParseIntoArray(QueuedCommands, TEXT(" && "), true);

				if (QueuedCommands.Num() > 0)
				{
					InCommand = QueuedCommands[0];
					QueuedCommands.RemoveAt(0);
				}
			}

			if (bEnableCommandHistory)
			{
				CommandHistoryIndex = INDEX_NONE;
				CommandHistory.Add(RawCommand);
			}

			return ActiveCommandObject->Internal_ProcessCommand(this, InCommand.ToLower());
		}

		if (bEnableCommandHistory && bAllowAllCommandsInHistory)
		{
			CommandHistoryIndex = INDEX_NONE;
			CommandHistory.Add(RawCommand);
		}
	}

	StopCommand(false, FText::Format(LOCTEXT("YetiOS_ProcessCommandFailed", "{0} error: invalid command."), FText::FromName(TerminalName)));
	return false;
}

class UYetiOS_TerminalCommand* UYetiOS_TerminalProgram::Internal_GetTerminalCommandObject(const FString& InTestCommand)
{
	if (CommandLibrary && InTestCommand.IsEmpty() == false)
	{
		TArray<UBlueprintGeneratedClass*> CommandsBlueprintGeneratedClass;
		CommandLibrary->GetObjects<UBlueprintGeneratedClass>(CommandsBlueprintGeneratedClass);

		if (CommandsBlueprintGeneratedClass.Num() > 0)
		{
			for (const auto& It : CommandsBlueprintGeneratedClass)
			{
				UYetiOS_TerminalCommand* MyCommandObject = NewObject<UYetiOS_TerminalCommand>(this, It);
				if (MyCommandObject->CommandEqualTo(InTestCommand))
				{
					return MyCommandObject;
				}
			}
		}
	}

	return nullptr;
}


class UYetiOS_DirectoryBase* UYetiOS_TerminalProgram::GetCurrentDirectory() const
{
	return CurrentDirectory;
}

#undef printlog_veryverbose
#undef LOCTEXT_NAMESPACE
