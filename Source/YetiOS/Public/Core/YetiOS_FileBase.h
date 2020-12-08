// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_FileBase.generated.h"

/*************************************************************************
* File Information:
YetiOS_FileBase.h

* Description:
Base class for all file types.
*************************************************************************/
UCLASS(hidedropdown, Blueprintable, DisplayName = "File")
class YETIOS_API UYetiOS_FileBase : public UObject
{
	GENERATED_BODY()

	FDelegateHandle DelegateHandle_OnAssociatedProgramInstalled;
	
protected:

	/** Name of this file without extension */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File")
	FText Name;

	/** Extension of this file. E.g: txt, mp3, wav etc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File")
	FText Extension;

	/** An icon for this file. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* Icon;

	/** Size in MB */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File", meta = (UIMin = "0", ClampMin = "0", UIMax = "10000"))
	float FileSizeInMB;

	/** Widget that represents this file. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	TSubclassOf<class UYetiOS_FileWidget> FileWidgetClass;

	/** Widget that represents the icon of this file. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	TSubclassOf<class UYetiOS_FileIconWidget> FileIconWidgetClass;

	/** Program to open when opening this file. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	TSubclassOf<class UYetiOS_BaseProgram> AssociatedProgramClass;

	/** If true, file is hidden. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bIsHidden : 1;

	/** If true, file can be deleted by user. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bIsDeletable : 1;

	/** If true, user can move this file to another directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bIsMovable : 1;

	/** If true, then if the associated program is running it will open in that instance. Eg: A video file will open on an existing media player (if already running) instead of opening a new one.
	* If false, then opening this file will also start associated program as a new instance (if the program is NOT set to run as single instance).
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bOpenInSameInstance : 1;

	/** Program that is associated with this file. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BaseProgram* AssociatedProgram;

	/** Reference to the widget that was created. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_FileWidget* FileWidget;

	/** Reference to the icon widget that was created. */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_FileIconWidget* FileIconWidget;

	/** Is the file currently open */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsOpen : 1;

public:

	UYetiOS_FileBase();

	/**
	* public static UYetiOS_FileBase::CreateFile
	* Creates the file in given directory. Directory must have bCanCreateNewFile set to true.
	* @param InParentDirectory [class UYetiOS_DirectoryBase*] Directory in which the file has to be created.
	* @param FileClass [TSubclassOf<UYetiOS_FileBase>] File class to create.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [UYetiOS_FileBase*] Returns newly created file.
	**/
	static UYetiOS_FileBase* CreateFile(class UYetiOS_DirectoryBase* InParentDirectory, TSubclassOf<UYetiOS_FileBase> FileClass, FYetiOsError& OutErrorMessage);

	/**
	* virtual public UYetiOS_FileBase::CloseFile
	* Close this file and removes widget from parent.
	**/
	virtual void CloseFile();

	/**
	* virtual public UYetiOS_FileBase::OpenFile
	* Opens this file in associated program. Will trigger ensure assert if parent directory is nullptr.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any)
	* @param bInForceOpenInSameInstance [const bool] True to force open in same instance of associated program.
	* @return [bool] True if the file was opened successfully.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS File")	
	virtual bool OpenFile(FYetiOsError& OutErrorMessage, const bool bInForceOpenInSameInstance = false);

	/**
	* virtual public UYetiOS_FileBase::RenameFile
	* Renames this file.
	* @param InNewName [const FText&] New name to change to.
	* @param OutErrorMessage [FYetiOsError&] Error message (if any).
	* @return [bool] True if file was renamed successfully.
	**/
	UFUNCTION(BlueprintCallable, Category = "Yeti OS File")	
	virtual bool RenameFile(const FText& InNewName, FYetiOsError& OutErrorMessage);

	/**
	* public UYetiOS_FileBase::GetFilename const
	* Returns the file name.
	* @param bWithExtension [const bool] Include extension. Eg: Sample.txt
	* @return [FText] Full name of this file.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	FText GetFilename(const bool bWithExtension) const;

	/**
	* public UYetiOS_FileBase::GetFileExtension const
	* Returns the extension part of this file. Example: txt
	* @return [FText] Extension of this file.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	FText GetFileExtension() const { return Extension; }

	/**
	* public UYetiOS_FileBase::GetFileIcon const
	* Returns icon of this file.
	* @return [class UObject*] Icon class. Can be UTexture or UMaterial
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	class UObject* GetFileIcon() const { return Icon; }

	/**
	* public UYetiOS_FileBase::GetFileSize const
	* Returns the file size in MB.
	* @return [float] File size.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	float GetFileSize() const { return FileSizeInMB; }

	/**
	* public UYetiOS_FileBase::IsHidden const
	* Is the file hidden?
	* @return [bool] True if file is hidden.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	bool IsHidden() const { return bIsHidden; }

	/**
	* public UYetiOS_FileBase::IsDeletable const
	* Is the file deletable by user?
	* @return [bool] True if file is deletable.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	bool IsDeletable() const { return bIsDeletable; }

	/**
	* public UYetiOS_FileBase::IsMovable const
	* Can this file be moved from one location to another?
	* @return [bool] True if file can be moved.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	bool IsMovable() const { return bIsMovable; }

	/**
	* public UYetiOS_FileBase::GetFileIconWidget const
	* Returns file icon widget.
	* @return [class UYetiOS_FileIconWidget*] File Icon Widget.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	class UYetiOS_FileIconWidget* GetFileIconWidget() const { return FileIconWidget; }

	/**
	* public UYetiOS_FileBase::IsOpen const
	* Is the file currently open?
	* @return [bool] True if this file is open in Associated Program.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	bool IsOpen() const { return bIsOpen; }

	/**
	* public UYetiOS_FileBase::GetFileWidget
	* Returns the file widget.
	* @return [class UYetiOS_FileWidget*] File Widget.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	class UYetiOS_FileWidget* GetFileWidget();

	/**
	* public UYetiOS_FileBase::GetParentDirectory const
	* Returns the directory where this file is located.
	* @return [class UYetiOS_DirectoryBase*] Parent directory.
	**/
	UFUNCTION(BlueprintPure, Category = "Yeti OS File")	
	class UYetiOS_DirectoryBase* GetParentDirectory() const;

	/**
	* public UYetiOS_FileBase::IsAssociatedProgramInstalled const
	* Checks if the associated program is installed on the Operating System.
	* @return [const bool] True if the program is installed.
	**/
	const bool IsAssociatedProgramInstalled() const;

private:

	/**
	* private UYetiOS_FileBase::Internal_OnFileCreate
	* Called when this file is created.
	* @See CreateFile.
	**/
	void Internal_OnFileCreate();

	/**
	* private UYetiOS_FileBase::Internal_OnAssociatedProgramInstalled
	* Automatically called when the associated program is installed on Operating System.
	* @param InNewInstalledProgram [class UYetiOS_BaseProgram*] Program which was installed.
	**/
	UFUNCTION()	
	void Internal_OnAssociatedProgramInstalled(class UYetiOS_BaseProgram* InNewInstalledProgram);

protected:

	/**
	* protected UYetiOS_FileBase::K2_OnFileCreate
	* Event called when this file is created.
	* @See UYetiOS_FileBase::Internal_OnFileCreate in C++
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS File", DisplayName = "On Create")	
	void K2_OnFileCreate();

	/**
	* protected UYetiOS_FileBase::K2_OnFileCreateFailed
	* Event called when creation of this file was failed.
	* @See UYetiOS_FileBase::CreateFile in C++
	* @param ErrorString [const FString &] Error message.
	**/
	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS File", DisplayName = "On Failed to Create")	
	void K2_OnFileCreateFailed(const FString& ErrorString);

public:

	/**
	* public UYetiOS_FileBase::GetFileContent
	* The actual file content. Must be implemented in Blueprint.
	* @return [class UYetiOS_FileContent*] File Content.
	**/
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Yeti OS File")	
	class UYetiOS_FileContent* GetFileContent();

public:

	FORCEINLINE TSubclassOf<class UYetiOS_FileIconWidget> GetFileIconWidgetClass() const { return FileIconWidgetClass; }
	FORCEINLINE const bool CanOpenInSameInstance() const {; return bOpenInSameInstance; }
	FORCEINLINE const bool IsSameFile(const UYetiOS_FileBase* OtherFile) const
	{
		return OtherFile->GetFilename(true).EqualToCaseIgnored(GetFilename(true));
	}
};
