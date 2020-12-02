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
		TSet<UYetiOS_FileBase*> AllFilesInParent = InParentDirectory->GetDirectoryFiles();
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
	}
	else
	{
		ErrorString = FString::Printf(TEXT("Failed to create file %s as it has no association with any program."), *ProxyFile->Name.ToString());
		bSuccess = false;
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

bool UYetiOS_FileBase::OpenFile(FYetiOsError& OutErrorMessage)
{
	checkf(GetParentDirectory(), TEXT("File requires a valid reference to directory to open"));
	bIsOpen = AssociatedProgram != nullptr;
	UYetiOS_Core* Local_OS = GetParentDirectory()->GetOwningOS();
	if (bIsOpen == false)
	{
		const FName ProgramIdentifier = AssociatedProgramClass->GetDefaultObject<UYetiOS_BaseProgram>()->GetProgramIdentifierName();
		bIsOpen = Local_OS->IsProgramInstalled(ProgramIdentifier, AssociatedProgram, OutErrorMessage);
	}

	if (bIsOpen)
	{
		bIsOpen = AssociatedProgram->OpenFile(this);
	}
	else
	{
		FYetiOsNotification NewNotification = FYetiOsNotification(EYetiOsNotificationCategory::CATEGORY_App, OutErrorMessage.ErrorException, OutErrorMessage.ErrorDetailedException, OutErrorMessage.ErrorCode);
		Local_OS->CreateOsNotification(NewNotification);
	}

	return bIsOpen;
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

FText UYetiOS_FileBase::GetFilename(const bool bWithExtension) const
{
	if (bWithExtension)
	{
		return FText::AsCultureInvariant(FString::Printf(TEXT("%s.%s"), *Name.ToString(), *Extension.ToString()));
	}

	return Name;
}

class UObject* UYetiOS_FileBase::GetDefaultIcon()
{
	if (OverrideDefaultIcon == nullptr)
	{
		UYetiOS_Core* Local_OwningOS = GetParentDirectory()->GetOwningOS();
		if (Local_OwningOS)
		{
			OverrideDefaultIcon = Local_OwningOS->GetDefaultIcon();
		}
	}

	return OverrideDefaultIcon;
}

class UYetiOS_FileWidget* UYetiOS_FileBase::GetFileWidget()
{
	if (FileWidget == nullptr)
	{
		FileWidget = UYetiOS_FileWidget::CreateFileWidget(FileWidgetClass, this);
	}

	return FileWidget;
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
	{
		DelegateHandle_OnAssociatedProgramInstalled = Local_OS->OnProgramInstalled.AddUObject(this, &UYetiOS_FileBase::Internal_OnAssociatedProgramInstalled);
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

bool UYetiOS_FileBase::RenameFile(const FText& InNewName)
{
	if (InNewName.EqualToCaseIgnored(Name))
	{
		return false;
	}

	Name = InNewName;
	return true;
}

#undef printlog_error
#undef printlog_veryverbose

#undef LOCTEXT_NAMESPACE
