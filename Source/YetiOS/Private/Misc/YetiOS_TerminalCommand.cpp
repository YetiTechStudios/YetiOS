// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Misc/YetiOS_TerminalCommand.h"
#include "Programs/YetiOS_TerminalProgram.h"
#include "Widgets/YetiOS_AppWidget.h"
#include "Widgets/YetiOS_DraggableWindowWidget.h"
#include "Core/YetiOS_Core.h"

UYetiOS_TerminalCommand::UYetiOS_TerminalCommand()
{
	MainCommand = "";
	MainCommandDescription = "";

	AlternateCommands = TArray<FString>();

	bFailIfParametersAreFound = true;
	bRequiresRootPermission = false;
	bValidateFlagsBeforeRunning = true;
}

const bool UYetiOS_TerminalCommand::Internal_ProcessCommand(class UYetiOS_TerminalProgram* InTerminal, const FString& InCommand)
{
	CurrentFullCommand = InCommand;
	OwningTerminal = InTerminal;
	FName MyTerminalName = OwningTerminal->GetTerminalName();
	if (MyTerminalName.IsNone())
	{
		MyTerminalName = FName("Yeti Terminal");
	}

	TerminalName = MyTerminalName.ToString();

	if (InCommand.StartsWith(InTerminal->GetRootCommand().ToString().ToLower()))
	{
		OwningTerminal->ChangeCurrentUser(OwningTerminal->GetOwningOS()->GetRootUser());
	}
	else
	{
		if (bRequiresRootPermission)
		{
			const FString MyErrorMessage = FString::Printf(TEXT("%s: error: '%s' permission denied."), *TerminalName, *MainCommand);
			OnFail(MyErrorMessage);
			return false;
		}

		OwningTerminal->ChangeCurrentUser(OwningTerminal->GetOwningOS()->GetCurrentUser());
	}

	const int32 TotalFlags = CommandFlags.Num();
	if (TotalFlags > 0)
	{
		/* ========================================================================================== */
		/* 
		* There is no guarantee that a developer will put flags with - or -- as prefix.
		* So we take extra care here to convert flags without prefixes to flags with prefixes.
		*/
		TMap<FString, FYetiOsTerminalCommand> TempCommandFlags;
		TempCommandFlags.Reserve(TotalFlags);
		for (const auto& It : CommandFlags)
		{			
			if (It.Key.StartsWith("--") == false && It.Key.StartsWith("-") == false)
			{
				TempCommandFlags.Add("-" + It.Key.ToLower(), It.Value);
			}
			else
			{
				TempCommandFlags.Add(It.Key.ToLower(), It.Value);
			}
		}

		CommandFlags = TempCommandFlags;
		/* ========================================================================================== */

		if (bValidateFlagsBeforeRunning)
		{
			FString InvalidFlag;
			if (ValidateCommandFlags(InvalidFlag) == false)
			{
				const FString MyErrorMessage = FString::Printf(TEXT("%s: error: Unknown flag %s."), *TerminalName, *InvalidFlag);
				OnFail(MyErrorMessage);
				return false;
			}
		}

		CommandFlags.GenerateKeyArray(AvailableFlags);
		for (const auto& It : CommandFlags)
		{
			if (It.Value.bIsRequired && HasFlag(It.Key) == false)
			{
				const FString MyErrorMessage = FString::Printf(TEXT("%s: error: %s requires %s flag."), *TerminalName, *MainCommand, *It.Key);
				OnFail(MyErrorMessage);
				return false;
			}
		}
	}
	else if (GetFlagsFromCommand().Num() > 0)
	{
		const FString MyErrorMessage = FString::Printf(TEXT("%s: error: %s does not support any flags."), *TerminalName, *MainCommand);
		OnFail(MyErrorMessage);
		return false;
	}
	

	CommandParameters = Internal_GetCommandParameters();
	if (bFailIfParametersAreFound && CommandParameters.Num() > 0)
	{
		const FString MyErrorMessage = FString::Printf(TEXT("%s: error: %s does not support any params."), *TerminalName, *MainCommand);
		OnFail(MyErrorMessage);
		return false;
	}

	if (bRequiresRootPermission)
	{
		PromptRootPassword();
	}
	
	return K2_OnProcessCommand(InCommand, bRequiresRootPermission);
}

void UYetiOS_TerminalCommand::Internal_ProcessCommandAsRoot()
{
	K2_OnProcessCommandAsRoot(CurrentFullCommand);
}

void UYetiOS_TerminalCommand::Internal_OnContinueExecution()
{
	K2_OnContinueExecution();
}

void UYetiOS_TerminalCommand::Internal_TerminateCommand()
{
	OwningTerminal->ChangeCurrentUser(OwningTerminal->GetOwningOS()->GetCurrentUser());
	OwningTerminal = nullptr;
	CommandFlags.Empty();
	CommandFlags.Compact();
	ConditionalBeginDestroy();
}

void UYetiOS_TerminalCommand::OnSuccess()
{
	OwningTerminal->StopCommand(true, FText::GetEmpty());
	Internal_TerminateCommand();
}

void UYetiOS_TerminalCommand::NotifyTerminal(const FText& InMessage, EYetiOsTerminalMessageLevel InMessageType)
{
	OwningTerminal->ReceiveMessageFromCommand(InMessage, InMessageType);
}

void UYetiOS_TerminalCommand::OnFail(const FString& InErrorMessage)
{
	OwningTerminal->StopCommand(false, FText::FromString(InErrorMessage));
	Internal_TerminateCommand();
}

const bool UYetiOS_TerminalCommand::OpenNewProgram(const FName& InProgramIdentifier, FYetiOsError& OutErrorMessage)
{
	return OwningTerminal->GetOwningOS()->StartProgram(InProgramIdentifier, OutErrorMessage);
}

void UYetiOS_TerminalCommand::CloseProgramByIdentifier(const FName& InProgramIdentifier)
{
	UYetiOS_BaseProgram* FoundProgram = nullptr;
	if (OwningTerminal->GetOwningOS()->IsProgramRunning(InProgramIdentifier, FoundProgram))
	{
		FYetiOsError OutErrorMessage;
		FoundProgram->CloseProgram(OutErrorMessage);
		FoundProgram = nullptr;
	}
}

void UYetiOS_TerminalCommand::CloseProgramByID(const int32& InProcessID)
{
	UYetiOS_BaseProgram* FoundProgram = nullptr;
	if (OwningTerminal->GetOwningOS()->IsProcessIdValid(InProcessID, FoundProgram))
	{
		FYetiOsError OutErrorMessage;
		FoundProgram->CloseProgram(OutErrorMessage);
		FoundProgram = nullptr;
	}
}

void UYetiOS_TerminalCommand::ExitTerminal(const bool bIsSuccess /*= true*/)
{
	OwningTerminal->StopCommand(bIsSuccess, FText::GetEmpty());
	FYetiOsError OutErrorMessage;
	OwningTerminal->CloseProgram(OutErrorMessage);
	Internal_TerminateCommand();
}

void UYetiOS_TerminalCommand::PromptRootPassword()
{
	OwningTerminal->PromptRootPassword();
}

FText UYetiOS_TerminalCommand::GetRootCommand() const
{
	return OwningTerminal->GetRootCommand();
}

const TArray<FString> UYetiOS_TerminalCommand::GetFlagsFromCommand() const
{
	const FString MyCommand = CurrentFullCommand.ToLower();
	TArray<FString> OutFlagsArray;
	MyCommand.ParseIntoArray(OutFlagsArray, TEXT(" "), true);

	TArray<FString> ReturnArray;
	for (const auto& It : OutFlagsArray)
	{
		if (It.StartsWith("--") || It.StartsWith("-"))
		{
			ReturnArray.Add(It);
		}
	}

	return ReturnArray;
}

const bool UYetiOS_TerminalCommand::ValidateCommandFlags(FString& OutInvalidFlag) const
{
	TArray<FString> OutKeys;
	CommandFlags.GenerateKeyArray(OutKeys);
	const auto MyCommandFlags = GetFlagsFromCommand();
	for (const auto& It : MyCommandFlags)
	{
		if (OutKeys.Contains(It) == false)
		{
			OutInvalidFlag = It;
			return false;
		}
	}

	return true;
}

const bool UYetiOS_TerminalCommand::HasFlag(const FString& InTestFlag) const
{
	if (AvailableFlags.Contains(InTestFlag))
	{
		const auto MyCommandFlags = GetFlagsFromCommand();
		return MyCommandFlags.Contains(InTestFlag);
	}

	return false;
}

inline const TArray<FString> UYetiOS_TerminalCommand::Internal_GetCommandParameters() const
{
	FString MyCommand = CurrentFullCommand.ToLower();
	MyCommand = MyCommand.Replace(*OwningTerminal->GetRootCommand().ToString(), TEXT(""));
	TArray<FString> OutStrings;
	MyCommand.ParseIntoArray(OutStrings, TEXT(" "), true);

	TArray<FString> ReturnResult;
	for (const auto& It : OutStrings)
	{
		if (It.StartsWith("-") == false && It != CurrentCommand)
		{
			ReturnResult.Add(It);
		}
	}

	return ReturnResult;
}

inline const bool UYetiOS_TerminalCommand::Internal_SetCurrentCommand(const FString& InTestCommand)
{
	FString MyCommand = InTestCommand.ToLower();

	// OwningTerminal variable is not yet valid here so trying to access it, will crash.
	UYetiOS_TerminalProgram* MyTerminal = Cast<UYetiOS_TerminalProgram>(GetOuter());

	MyCommand = MyCommand.Replace(*MyTerminal->GetRootCommand().ToString(), TEXT(""));
	TArray<FString> OutStrings;
	MyCommand.ParseIntoArray(OutStrings, TEXT(" "), true);

	if (OutStrings.IsValidIndex(0))
	{
		CurrentCommand = OutStrings[0];
		return true;
	}

	CurrentCommand = "";
	return false;
}

inline const bool UYetiOS_TerminalCommand::CommandEqualTo(const FString InTestCommand)
{
	if (Internal_SetCurrentCommand(InTestCommand))
	{
		if (MainCommand.ToLower() == CurrentCommand)
		{
			return true;
		}

		for (const auto& It : AlternateCommands)
		{
			if (It.ToLower() == CurrentCommand)
			{
				return true;
			}
		}
	}

	return false;
}
