// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/YetiOS_BaseProgram.h"
#include "YetiOS_Types.h"
#include "YetiOS_TerminalProgram.generated.h"

UCLASS(Abstract, DisplayName = "Terminal")
class YETIOS_API UYetiOS_TerminalProgram : public UYetiOS_BaseProgram
{
	GENERATED_BODY()
	
private:

	/* Name of this terminal */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Program")
	FName TerminalName;

	/* Default start directory from where this terminal should start. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Program")
	FName DefaultStartDirectory;

	/* List of commands this terminal can run. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Program")
	class UObjectLibrary* CommandLibrary;

	/* If enabled, terminal can run multiple commands. Example: command1 && command2 && command3. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Terminal Program", AdvancedDisplay)
	uint8 bSupportRunningMultiCommands : 1;

	/* True if this user is changed to root user. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsRunningAsRoot : 1;

	/* Current command this terminal is executing. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_TerminalCommand* ActiveCommandObject;

	/* Current directory path */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FString CurrentDirectoryPath;

	/* Reference to current directory */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_DirectoryBase* CurrentDirectory;

	/* Queued commands. Only valid if support for multi commands is enabled. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	TArray<FString> QueuedCommands;

	/* Current user. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	FYetiOsUser CurrentUser;
	
public:

	UYetiOS_TerminalProgram();

	/**
	* public static UYetiOS_TerminalProgram::GetRootCommandName
	* Returns the root command. By default in C++ this is defined as "sudo" inside YetiOS_Core file.
	* @return [const FText] Returns the root command.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")
	static const FText GetRootCommandName();

	virtual const bool StartProgram(FYetiOsError& OutErrorMessage) override;
	virtual void CloseProgram(FYetiOsError& OutErrorMessage, const bool bIsOperatingSystemShuttingDown = false) override;

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal")
	void CheckUserPrompt(const bool bIsYes);

	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal")
	void PromptForUserContinueInput();
	
	/**
	* public UYetiOS_TerminalProgram::ProcessCommand
	* Process the given command.
	* @param InCommand [FString] Command to process.
	* @return [const bool] True if the command was processed successfully.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal")	
	const bool ProcessCommand(FString InCommand);

	/**
	* public UYetiOS_TerminalProgram::ChangeCurrentPath
	* Updates the current path to the new one. If the given path does not exist, this will return false.
	* Can be used in cd command.
	* @param NewPath [const FString&] New path to change.
	* @return [const bool] True if current path was changed.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal")
	const bool ChangeCurrentPath(const FString& NewPath);

	/**
	* public UYetiOS_TerminalProgram::CheckRootPassword
	* Checks if the given password is the same as of root user (if available).
	* @param InPasswordToCheck [const FText&] Password to check.
	* @param bCaseSensitive [const bool] If false, ignore case.
	* @return [const bool] True if the password is same.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS Terminal", meta = (AdvancedDisplay = "1"))
	const bool CheckRootPassword(const FText& InPasswordToCheck, const bool bCaseSensitive = true);

	/**
	* public UYetiOS_TerminalProgram::GetCurrentUserName const
	* Gets the name of the current user.
	* @return [FText] Current user name.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	inline FText GetCurrentUserName() const { return CurrentUser.UserName; }

	/**
	* public UYetiOS_TerminalProgram::GetTerminalName const
	* Gets the name of this terminal.
	* @return [FName] Name of this terminal.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	inline FName GetTerminalName() const { return TerminalName; }

	/**
	* public UYetiOS_TerminalProgram::GetTotalCommands const
	* Gets the total commands supported by this terminal.
	* @return [const int32] Command count.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	const int32 GetTotalCommands() const;
	
	/**
	* public UYetiOS_TerminalProgram::HasPackageRepo const
	* Checks if the owning operating system has a package repo where you can install new programs.
	* @return [const bool] True if the owning operating system has a valid repo library.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	const bool HasPackageRepo() const;

	/**
	* public UYetiOS_TerminalProgram::GetAllCommands const
	* Gets an array of all the commands supported by this terminal.
	* @return [TArray<class UYetiOS_TerminalCommand*>] An array of commands valid for this terminal.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	TArray<class UYetiOS_TerminalCommand*> GetAllCommands() const;

	/**
	* public UYetiOS_TerminalProgram::ReceiveMessageFromCommand
	* Receives a message from command. This will then call K2_OnMessageReceivedFromCommand Blueprint event.
	* @See: UYetiOS_TerminalCommand::NotifyTerminal()
	* @param InMessage [const FText&] Message that was sent by the command.
	* @param InMessageType [EYetiOsTerminalMessageLevel] Message level.
	**/
	void ReceiveMessageFromCommand(const FText& InMessage, EYetiOsTerminalMessageLevel InMessageType);

	/**
	* public UYetiOS_TerminalProgram::StopCommand
	* Stops the current command. When the command is a success (UYetiOS_TerminalCommand::OnSuccess) or fail (UYetiOS_TerminalCommand::OnFail) it will call this method.
	* Stop command also supports running queued commands.
	* @See: This will call K2_OnCommandFinished.
	* @param bIsSuccess [const bool] If we should stop the command as success or fail.
	* @param InErrorMessage [const FText&] Any error message if its not success.
	**/
	void StopCommand(const bool bIsSuccess, const FText& InErrorMessage);

	/**
	* public UYetiOS_TerminalProgram::ChangeCurrentUser
	* Changes the user executing the command.
	* @param InNewUser [const FYetiOsUser&] New user to change to.
	**/
	void ChangeCurrentUser(const FYetiOsUser& InNewUser);

	/**
	* public UYetiOS_TerminalProgram::PromptRootPassword
	* This just calls K2_OnPromptRootPassword. It is up to the developer how to handle root password prompt.
	**/
	void PromptRootPassword();

private:

	/**
	* private UYetiOS_TerminalProgram::Internal_GetTerminalCommandObject
	* Gets the command object to execute based on given command. The command can be an alternate command too.
	* @param InTestCommand [const FString&] Command to check. This can be alternate command too.
	* @return [class UYetiOS_TerminalCommand*] Reference to the newly loaded command object or nullptr if the command was not found.
	**/
	class UYetiOS_TerminalCommand* Internal_GetTerminalCommandObject(const FString& InTestCommand);

protected:

	/**
	* protected UYetiOS_TerminalProgram::GetCurrentDirectory const
	* Gets the current directory this terminal is operating on.
	* Current directory can be changed using ChangeCurrentPath() function.
	* @return [class UYetiOS_DirectoryBase*] Reference to the current directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	class UYetiOS_DirectoryBase* GetCurrentDirectory() const;

	/**
	* protected UYetiOS_TerminalProgram::GetCurrentDirectoryPath const
	* Gets the current directory path. Can be changed using ChangeCurrentPath() function.
	* @return [const FText] Current Directory Path.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS Terminal")	
	const FText GetCurrentDirectoryPath() const { return FText::FromString(CurrentDirectoryPath); }	

	/**
	* protected UYetiOS_TerminalProgram::K2_OnMessageReceivedFromCommand
	* Event called when this terminal receives a message from terminal command.
	* @See ReceiveMessageFromCommand() function in C++.
	* @param OutMessage [const FText&] Message that was received
	* @param InMessageType [const EYetiOsTerminalMessageLevel&] Message level.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal", DisplayName = "OnMessageReceivedFromCommand")	
	void K2_OnMessageReceivedFromCommand(const FText& OutMessage, const EYetiOsTerminalMessageLevel& InMessageType);

	/**
	* protected UYetiOS_TerminalProgram::K2_OnCommandFinished
	* Event called when the current command has finished running.
	* @See StopCommand() function in C++.
	* @param bIsSuccess [const bool] True if the command was a success or no.
	* @param ErrorMessage [const FText&] Any error message if its not a success.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal", DisplayName = "OnCommandFinished")	
	void K2_OnCommandFinished(const bool bIsSuccess, const FText& ErrorMessage);

	/**
	* protected UYetiOS_TerminalProgram::K2_OnUserChanged
	* Event called when the user is changed.
	* @See ChangeCurrentUser() function in C++.
	* @param bIsRootUser [const bool] True if the new user is root user.
	* @param ActiveUser [const FYetiOsUser&] User that was changed to.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal", DisplayName = "OnUserChanged")	
	void K2_OnUserChanged(const bool bIsRootUser, const FYetiOsUser& ActiveUser);

	/**
	* protected UYetiOS_TerminalProgram::K2_OnPromptRootPassword
	* Event called when root password prompt is required.
	* @See PromptRootPassword() function in C++.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal", DisplayName = "OnPromptRootPassword")	
	void K2_OnPromptRootPassword();

	/**
	* protected UYetiOS_TerminalProgram::K2_OnPromptUserContinue
	* Event called when user input is required to continue the execution of command.
	* @See PromptForUserContinueInput() function in C++.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS Terminal", DisplayName = "OnPromptUserContinue")	
	void K2_OnPromptUserContinue();

public:
	
	/**
	* public UYetiOS_TerminalProgram::IsTerminalBusy const
	* Checks if the terminal is busy processing any command.
	* @return [FORCEINLINE const bool] True if terminal is busy.
	**/
	FORCEINLINE const bool IsTerminalBusy() const { return ActiveCommandObject != nullptr; }
};
