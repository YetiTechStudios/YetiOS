// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_Types.h"
#include "YetiOS_TerminalCommand.generated.h"

USTRUCT(BlueprintType)
struct FYetiOsTerminalCommand
{
	GENERATED_USTRUCT_BODY();
	
	/* If true, this flag is required. Command will not process if this flag is missing. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Terminal Command Flag Struct")
	bool bIsRequired;

	/* A help description for this command. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Yeti OS Terminal Command Flag Struct")
	FString Description;
};

UCLASS(Abstract, Blueprintable)
class YETIOS_API UYetiOS_TerminalCommand : public UObject
{
	GENERATED_BODY()
	
	friend class UYetiOS_TerminalProgram;
	
private:

	/* Name of this command. Example: cd, mkdir, exit etc. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command")
	FString MainCommand;

	/* A small description for this command. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command")
	FString MainCommandDescription;

	/* Array of alternate commands for running this command. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command")
	TArray<FString> AlternateCommands;

	/* 
	* Key - Flag. Must be prefixed by a - or --. Example: -a, --info etc.
	* Value - Detail for the flag.
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command")
	TMap<FString, FYetiOsTerminalCommand> CommandFlags;

	/* If true, then fail this command before processing if any parameters are found. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command")
	uint8 bFailIfParametersAreFound : 1;

	/* If true then you need to execute this command as root user. Example: sudo your_command. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command")
	uint8 bRequiresRootPermission : 1;

	/* If enabled, it will check if passed in flags are valid before processing. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Command", AdvancedDisplay)
	uint8 bValidateFlagsBeforeRunning : 1;

	/* Terminal which this command is running. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	class UYetiOS_TerminalProgram* OwningTerminal;

	/* Raw command without flags or parameters. 
	* Example: if its [ls -a] this will be [ls]
	* @See UYetiOS_TerminalCommand::Internal_SetCurrentCommand
	*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	FString CurrentCommand;

	/* An array of command parameters. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	TArray<FString> CommandParameters;

	/* Name of the owning terminal. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Debug, meta = (AllowPrivateAccess = "true"))
	FString TerminalName;

	/* CUrrent command that is being processed. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FString CurrentFullCommand;

	/* Array of flags available in this command. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<FString> AvailableFlags;

public:

	UYetiOS_TerminalCommand();

private:

	/**
	* private UYetiOS_TerminalCommand::Internal_ProcessCommand
	* Process the given command in given terminal.
	* @param InTerminal [class UYetiOS_TerminalProgram*] Terminal that should execute the command.
	* @param InCommand [const FString&] Command to execute. E.g: cd /home/desktop
	* @return [const bool] True if the command was processed successfully from Blueprint.
	**/
	const bool Internal_ProcessCommand(class UYetiOS_TerminalProgram* InTerminal, const FString& InCommand);

	/**
	* private UYetiOS_TerminalCommand::Internal_ProcessCommandAsRoot
	* Process the command as root user. E.g: sudo your_command
	**/
	void Internal_ProcessCommandAsRoot();

	/**
	* private UYetiOS_TerminalCommand::Internal_OnContinueExecution
	* Continues the command execution instead of stopping the command.
	* @See: UYetiOS_TerminalProgram::CheckUserPrompt
	**/
	void Internal_OnContinueExecution();

	/**
	* private UYetiOS_TerminalCommand::Internal_TerminateCommand
	* Destroys this command.
	**/
	void Internal_TerminateCommand();

protected:

	/**
	* protected UYetiOS_TerminalCommand::OnSuccess
	* Call this to notify the terminal that this command was a success.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")	
	void OnSuccess();

	/**
	* protected UYetiOS_TerminalCommand::NotifyTerminal
	* Notifies the terminal with a message. The terminal is then responsible for showing the message to the user.
	* @param InMessage [const FText&] Message to show.
	* @param InMessageType [EYetiOsTerminalMessageLevel] Message level type.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")	
	void NotifyTerminal(const FText& InMessage, EYetiOsTerminalMessageLevel InMessageType);

	/**
	* protected UYetiOS_TerminalCommand::OnFail
	* Call this to notify the terminal that this command has failed.
	* @param InErrorMessage [const FString&] A message explaining why this failed.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")	
	void OnFail(const FString& InErrorMessage);

	/**
	* protected UYetiOS_TerminalCommand::OpenNewProgram
	* Starts a new program with the given identifier. Program must be installed first to open.
	* @param InProgramIdentifier [const FName&] Program identifier to open.
	* @param OutErrorMessage [FYetiOsError&] Outputs error message (if any).
	* @return [const bool] True if the program was started successfully.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")
	const bool OpenNewProgram(const FName& InProgramIdentifier, FYetiOsError& OutErrorMessage);

	/**
	* protected UYetiOS_TerminalCommand::CloseProgramByIdentifier
	* Closes the given program identifier (if its running).
	* @param InProgramIdentifier [const FName&] Program identifier.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")
	void CloseProgramByIdentifier(const FName& InProgramIdentifier);

	/**
	* protected UYetiOS_TerminalCommand::CloseProgramByID
	* Closes a program associated with the given id.
	* @param InProcessID [const int32&] Process id to look for.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")
	void CloseProgramByID(const int32& InProcessID);

	/**
	* protected UYetiOS_TerminalCommand::ExitTerminal
	* Exits the terminal properly.
	* @param bIsSuccess [const bool] Exit as success or fail.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")	
	void ExitTerminal(const bool bIsSuccess = true);

	/**
	* protected UYetiOS_TerminalCommand::PromptRootPassword
	* Notifies the terminal to prompt for root password.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal Command")	
	void PromptRootPassword();

	/**
	* protected UYetiOS_TerminalCommand::GetFlagsFromCommand const
	* Returns an array of flags from command.
	* For example if you pass this command 'mkdir dir1 dir2 -h -v' this will return an array containing only -h and -v
	* @param InCommand [const FString&] Command to fetch flags from.
	* @return [const TArray<FString>] An array of flags from command.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")	
	const TArray<FString> GetFlagsFromCommand() const;

	/**
	* protected UYetiOS_TerminalCommand::ValidateCommandFlags const
	* Validates command flags from given command and outputs first found invalid flag.
	* @param InCommand [const FString&] Command to check flags for.
	* @param OutInvalidFlag [FString&] Invalid flag that is not supported by the command.
	* @return [const bool] Return true if no invalid flags were found.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")	
	const bool ValidateCommandFlags(FString& OutInvalidFlag) const;

	/**
	* protected UYetiOS_TerminalCommand::HasFlag const
	* Checks if the given command contains the given flag.
	* @param InTestFlag [const FString&] Flag to check. Must be prefixed with "--" or "-".
	* @return [const bool] Return true if flag was found.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")
	const bool HasFlag(const FString& InTestFlag) const;

	/**
	* protected UYetiOS_TerminalCommand::GetRootCommand const
	* Returns the root command as defined in OS.
	* @return [FText] Root command from OS.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")	
	FText GetRootCommand() const;

	/**
	* protected UYetiOS_TerminalCommand::GetMainCommand const
	* Returns the main command which you set in defaults.
	* @return [FString] Returns main command.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")
	inline FString GetMainCommand() const { return MainCommand; }

	/**
	* protected UYetiOS_TerminalCommand::GetMainCommandDescription const
	* Returns description for the main command
	* @return [FString] Returns main command description.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")
	inline FString GetMainCommandDescription() const { return MainCommandDescription; }

	/**
	* protected UYetiOS_TerminalCommand::GetAlternateCommands const
	* Returns an array of alternate commands for this command. (If any)
	* @return [TArray<FString>] Returns an array of alternate commands. (If available)
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")	
	inline TArray<FString> GetAlternateCommands() const { return AlternateCommands; }

	/**
	* protected UYetiOS_TerminalCommand::GetCommandFlags const
	* Returns supported command flags.
	* @return [TMap<FString, FYetiOsTerminalCommand>] Returns a TMap containing command flags.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal Command")	
	inline TMap<FString, FYetiOsTerminalCommand> GetCommandFlags() const { return CommandFlags; }

	/**
	* protected UYetiOS_TerminalCommand::K2_OnProcessCommand
	* Event called when a command is processed.
	* @See UYetiOS_TerminalCommand::Internal_ProcessCommand.
	* @param CommandToProcess [const FString&] Command to process.
	* @param bIsRunningAsRoot [const bool] True if this command is running as root user.
	* @return [const bool] True if this command has successfully processed. This should be true for 99.99% times.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal Command", DisplayName = "OnProcessCommand")
	const bool K2_OnProcessCommand(const FString& CommandToProcess, const bool bIsRunningAsRoot);

	/**
	* protected UYetiOS_TerminalCommand::K2_OnProcessCommandAsRoot
	* Event called when a command is processed as root user.
	* @See UYetiOS_TerminalCommand::Internal_ProcessCommandAsRoot()
	* @param CommandToProcess [const FString&] Command to process.
	* @return [const bool] True if this command has successfully processed. This should be true for 99.99% times.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal Command", DisplayName = "OnProcessCommandAsRoot")
	const bool K2_OnProcessCommandAsRoot(const FString& CommandToProcess);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Yeti OS Terminal Command", DisplayName = "OnContinueExecution")
	void K2_OnContinueExecution();

private:

	/**
	* private UYetiOS_TerminalCommand::Internal_GetCommandParameters const
	* Returns an array of parameters for given command.
	* Example: If you pass the command "mkdir folder1 folder2 -h -v" this method will return an array that contains just folder1 and folder2.
	* @param InTestCommand [const FString&] Command to test against.
	* @return [const TArray<FString>] An array that contains
	**/
	inline const TArray<FString> Internal_GetCommandParameters() const;

	/**
	* private UYetiOS_TerminalCommand::Internal_SetCurrentCommand const
	* Sets CurrentCommand variable to the proper command after stripping out parameters and flags.
	* @param InTestCommand [const FString&] Command to test.
	* @return [const bool] True if CurrentCommand variable was set.
	**/
	inline const bool Internal_SetCurrentCommand(const FString& InTestCommand);
	
public:

	/**
	* public UYetiOS_TerminalCommand::CommandEqualTo
	* Checks if the given command is equal to this object by checking against main command and alternate commands.
	* @param InTestCommand [const FString] Command to check.
	* @return [const bool] Returns true if either this is the main command or alternate command.
	**/
	inline const bool CommandEqualTo(const FString InTestCommand);
};
