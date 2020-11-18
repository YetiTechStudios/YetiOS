// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "YetiOS_FileBase.generated.h"


UCLASS(Abstract, Blueprintable, DisplayName = "File")
class YETIOS_API UYetiOS_FileBase : public UObject
{
	GENERATED_BODY()

	FDelegateHandle DelegateHandle_OnAssociatedProgramInstalled;
	
protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File")
	FText Name;

	/** Extension of this file. E.g: txt, mp3, wav etc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File")
	FText Extension;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	class UObject* Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File", AdvancedDisplay = "true", meta = (DisplayThumbnail = "true", AllowedClasses = "Texture,MaterialInterface"))
	mutable class UObject* OverrideDefaultIcon;

	/** Size in bytes */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Yeti OS File", meta = (UIMin = "0", ClampMin = "0", UIMax = "10000"))
	float Size;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	TSubclassOf<class UYetiOS_FileWidget> FileWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	TSubclassOf<class UYetiOS_FileIconWidget> FileIconWidgetClass;

	/** Program to open when opening this file. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	TSubclassOf<class UYetiOS_BaseProgram> AssociatedProgramClass;

	/* If true, file is hidden. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bIsHidden : 1;

	/* If true, file can be deleted by user. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bIsDeletable : 1;

	/* If true, user can move this file to another directory. */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS File")
	uint8 bIsMovable : 1;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_BaseProgram* AssociatedProgram;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_FileWidget* FileWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_FileIconWidget* FileIconWidget;

	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	class UYetiOS_DirectoryBase* ParentDirectory;

	/** Is the file currently open */
	UPROPERTY(VisibleInstanceOnly, Category = Debug)
	uint8 bIsOpen : 1;

public:

	UYetiOS_FileBase();

	static UYetiOS_FileBase* CreateFile(class UYetiOS_DirectoryBase* InParentDirectory, TSubclassOf<UYetiOS_FileBase> FileClass, FYetiOsError& OutErrorMessage);

	virtual bool OpenFile(FYetiOsError& OutErrorMessage);
	virtual void CloseFile();

	UFUNCTION(BlueprintCallable, Category = "Yeti OS File")
	virtual bool RenameFile(const FText& InNewName);

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	FText GetFilename(const bool bWithExtension) const;

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	FText GetFileExtension() const { return Extension; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	class UObject* GetFileIcon() const { return Icon; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	class UObject* GetDefaultIcon();

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	float GetFileSize() const { return Size; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	bool IsHidden() const { return bIsHidden; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	class UYetiOS_FileIconWidget* GetFileIconWidget() const { return FileIconWidget; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	bool IsOpen() const { return bIsOpen; }

	UFUNCTION(BlueprintPure, Category = "Yeti OS File")
	class UYetiOS_FileWidget* GetFileWidget();

	const bool IsAssociatedProgramInstalled() const;

private:

	void Internal_OnFileCreate();

	UFUNCTION()
	void Internal_OnAssociatedProgramInstalled(class UYetiOS_BaseProgram* InNewInstalledProgram);

protected:

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS File", DisplayName = "On Create")
	void K2_OnFileCreate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Yeti OS File", DisplayName = "On Failed to Create")
	void K2_OnFileCreateFailed(const FString& ErrorString);

public:

	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, Category = "Yeti OS File")
	class UObject* GetFileContent();

public:

	FORCEINLINE class UYetiOS_DirectoryBase* GetParentDirectory() const { return ParentDirectory; }
	FORCEINLINE TSubclassOf<class UYetiOS_FileIconWidget> GetFileIconWidgetClass() const { return FileIconWidgetClass; }
	FORCEINLINE const bool IsSameFile(const UYetiOS_FileBase* OtherFile) const
	{
		return OtherFile->GetFilename(true).EqualToCaseIgnored(GetFilename(true));
	}
};
