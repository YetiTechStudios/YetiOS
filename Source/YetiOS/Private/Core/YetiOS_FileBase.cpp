// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_FileBase.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Widgets/YetiOS_FileWidget.h"
#include "Widgets/YetiOS_FileIconWidget.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Hardware/YetiOS_Motherboard.h"
#include "Hardware/YetiOS_HardDisk.h"


DEFINE_LOG_CATEGORY_STATIC(LogYetiOsFile, All, All)

#define printlog_error(Param1)					UE_LOG(LogYetiOsFile, Warning, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)			UE_LOG(LogYetiOsFile, VeryVerbose, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_FileBase::UYetiOS_FileBase()
{
	Name = FText::GetEmpty();
	Extension  = FText::AsCultureInvariant("");
	Icon = nullptr;
	FileSizeInMB = 0.f;
	bIsHidden = false;
	bIsDeletable = true;
	bIsMovable = true;
	bIsOpen = false;
}

UYetiOS_FileBase* UYetiOS_FileBase::CreateFile(class UYetiOS_DirectoryBase* InParentDirectory, TSubclassOf<UYetiOS_FileBase> FileClass, FYetiOsError& OutErrorMessage)
{
	checkf(InParentDirectory, TEXT("File requires a valid reference to directory"));
	
	bool bSuccess = true;
	FString ErrorString = "";
	OutErrorMessage = FYetiOsError();
	UYetiOS_FileBase* ProxyFile = NewObject<UYetiOS_FileBase>(InParentDirectory, FileClass);
	if (ProxyFile->AssociatedProgramClass)
	{
		const TSet<UYetiOS_FileBase*>& AllFilesInParent = InParentDirectory->GetDirectoryFiles();
		for (const auto& It : AllFilesInParent)
		{
			if (It->IsSameFile(ProxyFile))
			{
				ErrorString = FString::Printf(TEXT("File %s exists in directory %s."), *ProxyFile->GetFilename(true).ToString(), *InParentDirectory->GetDirectoryName().ToString());
				bSuccess = false;
				break;
			}
		}

		if (InParentDirectory->GetOwningOS()->GetOwningDevice()->GetMotherboard()->GetHardDisk()->HasEnoughSpace(ProxyFile->FileSizeInMB) == false)
		{
			ErrorString = FString::Printf(TEXT("Not enough space to create %s."), *ProxyFile->GetFilename(true).ToString());
			bSuccess = false;
		}

		if (InParentDirectory->CanCreateNewFile() == false)
		{
			ErrorString = FString::Printf(TEXT("Permission denied %s in %s."), *ProxyFile->GetFilename(true).ToString(), *InParentDirectory->GetDirectoryName().ToString());
			bSuccess = false;
		}
	}

	if (bSuccess)
	{
		ProxyFile->FileIconWidget = UYetiOS_FileIconWidget::CreateFileIconWidget(ProxyFile, OutErrorMessage);
		ProxyFile->Internal_OnFileCreate();
		printlog_veryverbose(FString::Printf(TEXT("Created file %s in directory %s"), *ProxyFile->Name.ToString(), *InParentDirectory->GetDirectoryName().ToString()));
	}
	else
	{
		OutErrorMessage = GetErrorStruct(FText::GetEmpty(), FText::FromString(ErrorString), FText::GetEmpty());
		printlog_error(ErrorString);
		ProxyFile->K2_OnFileCreateFailed(ErrorString);
		ProxyFile->ConditionalBeginDestroy();
		ProxyFile = nullptr;
	}

	return ProxyFile;
}

void UYetiOS_FileBase::CloseFile()
{
	bIsOpen = false;

	if (DelegateHandle_OnAssociatedProgramInstalled.IsValid())
	{
		UYetiOS_Core* Local_OS = GetParentDirectory()->GetOwningOS();
		Local_OS->OnProgramInstalled.Remove(DelegateHandle_OnAssociatedProgramInstalled);
		DelegateHandle_OnAssociatedProgramInstalled.Reset();
	}

	AssociatedProgram = nullptr;

	if (FileWidget)
	{
		FileWidget->RemoveFromParent();
		FileWidget->ConditionalBeginDestroy();
		FileWidget = nullptr;
	}
}

bool UYetiOS_FileBase::OpenFile(FYetiOsError& OutErrorMessage, const bool bInForceOpenInSameInstance /*= false*/)
{
	if (ensureMsgf(GetParentDirectory() != nullptr, TEXT("File requires a valid reference to directory to open")))
	{
		bIsOpen = AssociatedProgram != nullptr;
		UYetiOS_Core* Local_OS = GetParentDirectory()->GetOwningOS();
		if (bIsOpen == false)
		{
			const FName ProgramIdentifier = AssociatedProgramClass->GetDefaultObject<UYetiOS_BaseProgram>()->GetProgramIdentifierName();
			bIsOpen = Local_OS->IsProgramInstalled(ProgramIdentifier, AssociatedProgram, OutErrorMessage);
		}

		if (bIsOpen)
		{
			bIsOpen = AssociatedProgram->OpenFile(this, bInForceOpenInSameInstance);
		}
		else
		{
			FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, OutErrorMessage.ErrorException, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode);
			Local_OS->CreateOsNotification(NewNotification);
		}

		return bIsOpen;
	}

	return false;
}

FText UYetiOS_FileBase::GetFilename(const bool bWithExtension) const
{
	if (bWithExtension)
	{
		return FText::AsCultureInvariant(FString::Printf(TEXT("%s.%s"), *Name.ToString(), *Extension.ToString()));
	}

	return Name;
}

class UYetiOS_FileWidget* UYetiOS_FileBase::GetFileWidget()
{
	if (FileWidget == nullptr)
	{
		FileWidget = UYetiOS_FileWidget::CreateFileWidget(FileWidgetClass, this);
	}

	return FileWidget;
}

class UYetiOS_DirectoryBase* UYetiOS_FileBase::GetParentDirectory() const
{
	return Cast<class UYetiOS_DirectoryBase>(GetOuter());
}

const bool UYetiOS_FileBase::IsAssociatedProgramInstalled() const
{
	if (AssociatedProgram)
	{
		UYetiOS_Core* Local_OwningOS = GetParentDirectory()->GetOwningOS();
		return Local_OwningOS->IsProgramInstalled(AssociatedProgram->GetProgramIdentifierName());
	}

	return false;
}

void UYetiOS_FileBase::Internal_OnFileCreate()
{
	K2_OnFileCreate();
	FYetiOsError OutError;
	const FName ProgramIdentifier = AssociatedProgramClass->GetDefaultObject<UYetiOS_BaseProgram>()->GetProgramIdentifierName();
	UYetiOS_Core* Local_OS = GetParentDirectory()->GetOwningOS();
	Local_OS->IsProgramInstalled(ProgramIdentifier, AssociatedProgram, OutError);
	if (AssociatedProgram == nullptr)

void UYetiOS_FileBase::Internal_OnAssociateProgramClass()
{
	if (AssociatedProgramClass)
	{
		FYetiOsError OutError;
		const FName ProgramIdentifier = AssociatedProgramClass->GetDefaultObject<UYetiOS_BaseProgram>()->GetProgramIdentifierName();
		UYetiOS_Core* Local_OS = GetParentDirectory()->GetOwningOS();
		Local_OS->IsProgramInstalled(ProgramIdentifier, AssociatedProgram, OutError);
		if (AssociatedProgram == nullptr)
		{
			DelegateHandle_OnAssociatedProgramInstalled = Local_OS->OnProgramInstalled.AddUObject(this, &UYetiOS_FileBase::Internal_OnAssociatedProgramInstalled);
		}
	}
}

void UYetiOS_FileBase::Internal_OnAssociatedProgramInstalled(class UYetiOS_BaseProgram* InNewInstalledProgram)
{
	if (AssociatedProgramClass == InNewInstalledProgram->GetClass())
	{
		AssociatedProgram = InNewInstalledProgram;
		FileIconWidget->AssociatedProgramInstalled();
		UYetiOS_Core* Local_OS = GetParentDirectory()->GetOwningOS();
		Local_OS->OnProgramInstalled.Remove(DelegateHandle_OnAssociatedProgramInstalled);
		DelegateHandle_OnAssociatedProgramInstalled.Reset();
	}
}

bool UYetiOS_FileBase::RenameFile(const FText& InNewName, FYetiOsError& OutErrorMessage)
{
	if (InNewName.EqualToCaseIgnored(Name))
	{
		return false;
	}

	const TSet<UYetiOS_FileBase*>& AllFilesInParent = GetParentDirectory()->GetDirectoryFiles();
	for (const auto& It : AllFilesInParent)
	{
		if (It->IsSameFile(this))
		{
			const FString Title = "File Exists";
			const FString Description = FString::Printf(TEXT("File '%s' alrady exists in directory '%s'."), *GetFilename(true).ToString(), *GetParentDirectory()->GetDirectoryName().ToString());
			OutErrorMessage = GetErrorStruct(FText::FromString("ERR_FILE_EXISTS"), FText::FromString(Title), FText::FromString(Description));
			return false;
		}
	}

	Name = InNewName;
	return true;
}

void UYetiOS_FileBase::SetAssociatedProgramClass(TSubclassOf<class UYetiOS_BaseProgram> NewProgramClass)
{
	AssociatedProgramClass = NewProgramClass;
	Internal_OnAssociateProgramClass();
}

#undef printlog_error
#undef printlog_veryverbose

#undef LOCTEXT_NAMESPACE
