// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_DirectoryBase.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryRoot.h"
#include "Devices/YetiOS_BaseDevice.h"
#include "Devices/YetiOS_DeviceManagerActor.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsDirectoryBase, All, All)

#define printlog_warning(Param1)					UE_LOG(LogYetiOsDirectoryBase, Warning, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)				UE_LOG(LogYetiOsDirectoryBase, VeryVerbose, TEXT("%s"), *FString(Param1))

#define LOCTEXT_NAMESPACE "YetiOS"

UYetiOS_DirectoryBase::UYetiOS_DirectoryBase()
{
	bCanCreateNewFolder = true;
	bCanCreateNewFile = true;
	bIsHidden = false;
	ParentDirectory = nullptr;
	bIsSystemDirectory = false;
	DirectoryType = EDirectoryType::Other;
}

UYetiOS_DirectoryBase* UYetiOS_DirectoryBase::GetChildDirectory(TSubclassOf<UYetiOS_DirectoryBase> InDirectoryClass) const
{
	for (const auto& Itr : ChildDirectories)
	{
		if (Itr->GetClass() == InDirectoryClass)
		{
			return Itr;
		}
	}

	return nullptr;
}

UYetiOS_DirectoryBase* UYetiOS_DirectoryBase::GetChildDirectoryByType(EDirectoryType InType) const
{
	UYetiOS_DirectoryRoot* MyRootDirectory = OwningOS->GetRootDirectory();
	return MyRootDirectory->Internal_GetChildDirectoryByType(InType);
}

UYetiOS_DirectoryBase* UYetiOS_DirectoryBase::GetChildDirectoryByName(const FName& InDirectoryName, const bool bRecursive /*= true*/) const
{
	if (ChildDirectories.Num() > 0 && InDirectoryName.IsNone() == false)
	{
		const FText DirectoryNameToCheck = FText::FromName(InDirectoryName).ToLower();

		// First check the child directories only. If its there then return that.
		for (const auto& It : ChildDirectories)
		{
			const FText ItDirectoryName = It->DirectoryName.ToLower();
			if (ItDirectoryName.EqualToCaseIgnored(DirectoryNameToCheck))
			{
				return It;
			}
		}

		if (bRecursive)
		{
			for (auto const& It : ChildDirectories)
			{
				for (auto const& ChildIt : It->ChildDirectories)
				{
					const FText ChildItDirectoryName = ChildIt->DirectoryName.ToLower();
					if (ChildItDirectoryName.EqualToCaseIgnored(DirectoryNameToCheck))
					{
						return ChildIt;
					}

					ChildIt->GetChildDirectoryByName(InDirectoryName);
				}
			}
		}
	}

	return nullptr;
}

UYetiOS_DirectoryBase* UYetiOS_DirectoryBase::GetDirectoryFromPath(const FString& InPath) const
{
	if (InPath.IsEmpty() == false)
	{
		if (OwningOS->GetRootDirectory()->GetDirectoryName().ToString().ToLower() == InPath.ToLower() || InPath == UYetiOS_Core::PATH_DELIMITER)
		{
			return OwningOS->GetRootDirectory();
		}

		if (InPath.StartsWith(UYetiOS_Core::PATH_DELIMITER))
		{
			UYetiOS_DirectoryBase* ReturnResult = nullptr;
			if (OwningOS->DirectoryExists(InPath, ReturnResult))
			{
				return ReturnResult;
			}
		}
		else
		{
			return GetChildDirectoryByName(FName(*InPath), false);
		}
	}

	return nullptr;
}

UYetiOS_DirectoryBase* UYetiOS_DirectoryBase::CreateChildDirectory(class UYetiOS_Core* InOwningOS, TSubclassOf<UYetiOS_DirectoryBase> InDirectoryClass, FYetiOsError& OutErrorMessage, const FText& InDirectoryName, const bool bCreateHidden /*= false*/, const bool bForceCreate /*= false*/)
{
	TArray<TSubclassOf<UYetiOS_DirectoryBase>> TargetArray;
	TargetArray.Add(InDirectoryClass);
	
	UYetiOS_DirectoryBase* ReturnResult = nullptr;
	TArray<UYetiOS_DirectoryBase*> ReturnArray = Internal_CreateChildDirectories(InOwningOS, TargetArray, OutErrorMessage, bForceCreate, false, InDirectoryName);
	if (ReturnArray.IsValidIndex(0))
	{
		if (OutErrorMessage.ErrorCode.IsEmptyOrWhitespace() == false)
		{
			ReturnArray[0]->ConditionalBeginDestroy();
		}
		else
		{
			ReturnResult = ReturnArray[0];
		}
	}

	return ReturnResult;
}

TArray<UYetiOS_DirectoryBase*> UYetiOS_DirectoryBase::CreateChildDirectories(class UYetiOS_Core* InOwningOS, const TArray<TSubclassOf<UYetiOS_DirectoryBase>>& InDirectoryClasses, FYetiOsError& OutErrorMessage, const bool bForceCreate /* = false */)
{
	return Internal_CreateChildDirectories(InOwningOS, InDirectoryClasses, OutErrorMessage, bForceCreate, false);
}

TArray<UYetiOS_DirectoryBase*> UYetiOS_DirectoryBase::CreateNativeChildDirectories(class UYetiOS_Core* InOwningOS, FYetiOsError& OutErrorMessage, const bool bForceCreate /*= false*/, const bool bCreateGrandChildDirectories /*= true*/)
{
	return Internal_CreateChildDirectories(InOwningOS, ChildDirectoryClasses, OutErrorMessage, bForceCreate, bCreateGrandChildDirectories, FText::GetEmpty(), true);
}

void UYetiOS_DirectoryBase::EnsureOS(const class UYetiOS_Core* InOS)
{
	check(InOS);
	if (OwningOS == nullptr)
	{
		OwningOS = const_cast<UYetiOS_Core*>(InOS);
	}
}

const bool UYetiOS_DirectoryBase::CreateNewFile(FFileProperties InNewFile, FYetiOsError& OutErrorMessage, const bool bForceCreate /*= false*/)
{
	bool bLocal_CreateFile = bCanCreateNewFile;

	if (bForceCreate)
	{
		bLocal_CreateFile = true;
	}

	if (bLocal_CreateFile)
	{
		bool bFileExists = false;
		for (const auto& It : Files)
		{
			const FString ExistingFile = FString::Printf(TEXT("%s%s"), *It.FileName.ToString(), *It.FileExtension.ToString());
			const FString TargetFile = FString::Printf(TEXT("%s%s"), *InNewFile.FileName.ToString(), *InNewFile.FileExtension.ToString());
			if (ExistingFile.ToLower() == TargetFile.ToLower())
			{
				bFileExists = true;
				OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_FileCreateAlreadyExists", "ERR_FILE_EXISTS");
				OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_FileCreateAlreadyExistsException", "Cannot create file inside {0}."), DirectoryName);
				OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_FileCreateAlreadyExistsDetailedException", "A file with name {0} already exists inside {1}."), InNewFile.FileName, DirectoryName);
				break;
			}
		}

		if (bFileExists == false)
		{
			Files.Add(InNewFile);
			return true;
		}
	}
	else
	{
		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_FileCreateNoPermisson", "ERR_PERMISSION_DENIED");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_FileCreateNoPermissonException", "Cannot create file inside {0}."), DirectoryName);
		OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_FileCreateNoPermissonDetailedException", "Permission error. Please make sure that '{0}' directory can create new folder."), DirectoryName);
	}

	return false;
}

TArray<UYetiOS_DirectoryBase*> UYetiOS_DirectoryBase::Internal_CreateChildDirectories(class UYetiOS_Core* InOwningOS, const TArray<TSubclassOf<UYetiOS_DirectoryBase>>& InDirectoryClasses, FYetiOsError& OutErrorMessage, const bool bForceCreate /*= false*/, const bool bCreateGrandChildDirectories /*= true*/, const FText& CheckDirectoryName /*= FText::GetEmpty()*/, const bool bIsSystemDir /*= false*/)
{
	EnsureOS(InOwningOS);
	TArray<UYetiOS_DirectoryBase*> ReturnResult;
	bool bLocal_CanCreateNewDir = bCanCreateNewFolder;

	if (bForceCreate)
	{
		bLocal_CanCreateNewDir = true;
	}

	if (bLocal_CanCreateNewDir)
	{
		UYetiOS_DirectoryBase* DummyOut;
		if (HasChildDirectory(CheckDirectoryName, DummyOut))
		{
			OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_DirectoryCreateDuplicateDir", "ERR_DIRECTORY_EXISTS");
			OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_DirectoryCreateDuplicateDirException", "A directory with name '{0}' already exists in '{1}'."), CheckDirectoryName, DirectoryName);
			OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_DirectoryCreateDuplicateDirDetailedException", "User cannot create '{0}' inside {1}. Directory already exists."), CheckDirectoryName, DirectoryName);
			// #TERMINALPLUGIN TODO Add to OS notification.
		}
		else
		{
			for (int32 i = 0; i < InDirectoryClasses.Num(); ++i)
			{
				UYetiOS_DirectoryBase* ChildDirectory = NewObject<UYetiOS_DirectoryBase>(this, InDirectoryClasses[i]);
				ChildDirectory->ParentDirectory = this;
				if (CheckDirectoryName.IsEmptyOrWhitespace() == false)
				{
					ChildDirectory->DirectoryName = CheckDirectoryName;
				}
				ChildDirectory->bIsSystemDirectory = false;
				ChildDirectories.Add(ChildDirectory);
				if (bIsSystemDir)
				{
					ChildDirectory->bIsSystemDirectory = true;
					SystemDirectories.Add(ChildDirectory);
				}
				ReturnResult.Add(ChildDirectory);
				OwningOS->AddToCreatedDirectories(ChildDirectory);
				printlog_veryverbose(FString::Printf(TEXT("Created child directory [%s] in %s."), *ChildDirectory->DirectoryName.ToString(), *DirectoryName.ToString()));
				if (bCreateGrandChildDirectories)
				{
					ChildDirectory->Internal_CreateChildDirectories(InOwningOS, ChildDirectory->ChildDirectoryClasses, OutErrorMessage, bForceCreate);
				}
			}
		}
	}
	else
	{
		OutErrorMessage.ErrorCode = LOCTEXT("YetiOS_DirectoryCreateFailNoPermisson", "ERR_PERMISSION_DENIED");
		OutErrorMessage.ErrorException = FText::Format(LOCTEXT("YetiOS_DirectoryCreateFailNoPermissonException", "Cannot create folder inside {0}."), DirectoryName);
		OutErrorMessage.ErrorDetailedException = FText::Format(LOCTEXT("YetiOS_DirectoryCreateFailNoPermissonDetailedException", "User cannot create folder due to permission error. Please make sure that {0} directory can create new folder."), DirectoryName);
		// #TERMINALPLUGIN TODO Add to OS notification.
	}

	return ReturnResult;
}

UYetiOS_DirectoryBase* UYetiOS_DirectoryBase::Internal_GetChildDirectoryByType(const EDirectoryType& InType) const
{
	for (auto const& It : ChildDirectories)
	{
		if (It->DirectoryType == InType)
		{
			return It;
		}

		// Look in child directories too.
		for (auto const& ChildIt : It->ChildDirectories)
		{
			if (ChildIt->DirectoryType == InType)
			{
				return ChildIt;
			}

			UYetiOS_DirectoryBase* Local_Child = ChildIt->Internal_GetChildDirectoryByType(InType);
			if (Local_Child)
			{
				return Local_Child;
			}
		}
	}

	return nullptr;
}

void UYetiOS_DirectoryBase::DestroyDirectory()
{
	for (const auto& It : ChildDirectories)
	{
		It->DestroyDirectory();
	}
	ChildDirectories.Empty();
	ParentDirectory = nullptr;
	printlog_veryverbose(FString::Printf(TEXT("Destroying directory %s"), *DirectoryName.ToString()));
	ConditionalBeginDestroy();
}

FText UYetiOS_DirectoryBase::GetDirectoryPath() const
{
	return FText::FromString(GetFullPath(true));
}

inline FString UYetiOS_DirectoryBase::GetFullPath(const bool bDoNoIncludeHome /*= false*/) const
{
	UYetiOS_DirectoryBase* CurrentDirectory = const_cast<UYetiOS_DirectoryBase*>(this);
	UYetiOS_DirectoryRoot* MyRootDirectory = OwningOS->GetRootDirectory();

	if (MyRootDirectory == Cast<UYetiOS_DirectoryRoot>(CurrentDirectory))
	{
		return UYetiOS_Core::PATH_DELIMITER;
	}

	FString FullPath = "";
	TArray<FText> Paths;	
	bool bReachedRootDirectory = false;
	while (bReachedRootDirectory == false)
	{
		CurrentDirectory = CurrentDirectory->ParentDirectory;
		Paths.Add(CurrentDirectory->DirectoryName);
		if (MyRootDirectory == Cast<UYetiOS_DirectoryRoot>(CurrentDirectory))
		{
			bReachedRootDirectory = true;
		}
	}

	Algo::Reverse(Paths);
	for (auto const& It : Paths)
	{
		if (It.ToString() != MyRootDirectory->DirectoryName.ToString())
		{
			FullPath += (It.ToString() + UYetiOS_Core::PATH_DELIMITER);
		}
	}

	if (bDoNoIncludeHome)
	{
		static const FString HomePathName = "home/";
		if (FullPath.StartsWith(HomePathName))
		{
			FullPath.RemoveFromStart(HomePathName);
		}
	}

	return FString::Printf(TEXT("%s%s%s"), *UYetiOS_Core::PATH_DELIMITER, *FullPath, *DirectoryName.ToString());
}

inline const bool UYetiOS_DirectoryBase::HasChildDirectory(const FText& InDirectoryName, UYetiOS_DirectoryBase*& OutFoundDirectory, const bool bIgnoreCase /*= true*/) const
{
	if (InDirectoryName.IsEmptyOrWhitespace() == false)
	{
		for (const auto& It : ChildDirectories)
		{
			const bool bHasDirectory = bIgnoreCase ? It->DirectoryName.EqualToCaseIgnored(InDirectoryName) : It->DirectoryName.EqualTo(InDirectoryName);
			if (bHasDirectory)
			{
				OutFoundDirectory = It;
				return true;
			}
		}
	}

	OutFoundDirectory = nullptr;
	return false;
}

#undef printlog_warning
#undef printlog_veryverbose
#undef LOCTEXT_NAMESPACE
