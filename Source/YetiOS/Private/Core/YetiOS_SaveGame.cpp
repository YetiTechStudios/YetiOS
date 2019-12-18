// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_SaveGame.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Devices/YetiOS_PortableDevice.h"
#include "Templates/SubclassOf.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsSaveGame, All, All)

#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsOperatingSystem, VeryVerbose, TEXT("%s"), *FString(Param1))


UYetiOS_SaveGame::UYetiOS_SaveGame()
{
	SaveSlotName = "YetiTechYetiOsPlugin";
	UserIndex = 0;
}

const bool UYetiOS_SaveGame::SaveGame(const class UYetiOS_BaseDevice* InDevice)
{
	if (InDevice)
	{
		const AYetiOS_DeviceManagerActor* MyDeviceManager = Cast<AYetiOS_DeviceManagerActor>(InDevice->GetOuter());
		if (MyDeviceManager && MyDeviceManager->CanSaveGame())
		{
			UYetiOS_SaveGame* SaveGameInstance = Cast<UYetiOS_SaveGame>(UGameplayStatics::CreateSaveGameObject(UYetiOS_SaveGame::StaticClass()));
			SaveGameInstance->DeviceData.bSaveLoad_OsInstalled = InDevice->IsOperatingSystemInstalled();

			const UYetiOS_PortableDevice* MyPortableDevice = Cast<UYetiOS_PortableDevice>(InDevice);
			if (MyPortableDevice)
			{
				SaveGameInstance->DeviceData.SaveLoad_BatteryLevel = MyPortableDevice->GetBatteryLevel();
			}

			const UYetiOS_Core* OperatingSystem = InDevice->GetOperatingSystem();
			if (OperatingSystem)
			{
				SaveGameInstance->OsData.SaveLoad_OsUsers = OperatingSystem->GetAllUsers();
				SaveGameInstance->OsData.SaveLoad_OSVersion = OperatingSystem->GetOsVersion();
				SaveGameInstance->OsData.SaveLoad_RemainingSpace = OperatingSystem->GetRemainingSpace();
				const TArray<const UYetiOS_DirectoryBase*> AllDirectories = OperatingSystem->GetAllCreatedDirectories();

				for (const auto& It : AllDirectories)
				{
					FYetiOsDirectorySaveLoad DirectorySave;
					DirectorySave.SaveLoad_DirPath = It->GetFullPath();
					DirectorySave.SaveLoad_DirectoryName = It->GetDirectoryName();
					DirectorySave.SaveLoad_DirectoryIcon = It->GetDirectoryIcon();
					DirectorySave.bSaveLoad_CanCreateNewFolder = It->CanCreateNewFolder();
					DirectorySave.bSaveLoad_CanCreateNewFile = It->CanCreateNewFile();
					DirectorySave.bSaveLoad_IsHidden = It->IsHidden();
					DirectorySave.SaveLoad_Files = It->GetDirectoryFiles();
					DirectorySave.SaveLoad_ChildDirectoryClasses = It->GetChildDirectories();

					const int32 AddedIndex = SaveGameInstance->DirectoryData.Add(DirectorySave);
					printlog_veryverbose(FString::Printf(TEXT("Directory [%s] saved at index %i"), *DirectorySave.SaveLoad_DirPath, AddedIndex));
				}

				const TArray<UYetiOS_BaseProgram*> AllInstalledPrograms = OperatingSystem->GetInstalledPrograms();
				for (const auto& It : AllInstalledPrograms)
				{
					if (It->IsSystemInstalledProgram() == false)
					{
						FYetiOsProgramSaveLoad ProgramSave;
						ProgramSave.SaveLoad_ProgramClass = It->GetClass();
						ProgramSave.SaveLoad_ProgramName = It->GetProgramName();
						ProgramSave.SaveLoad_ProgramIdentifier = It->GetProgramIdentifierName();
						ProgramSave.SaveLoad_ProgramIcon = It->GetProgramIcon();
						ProgramSave.SaveLoad_ProgramSpace = It->GetProgramSpace();
						ProgramSave.bSaveLoad_SingleInstanceOnly = It->IsSingleInstanceProgram();

						const int32 AddedIndex = SaveGameInstance->ProgramData.Add(ProgramSave);
						printlog_veryverbose(FString::Printf(TEXT("Program [%s] saved at index %i"), *ProgramSave.SaveLoad_ProgramName.ToString(), AddedIndex));
					}
				}
			}

			return UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);
		}
	}

	return false;
}

const UYetiOS_SaveGame* UYetiOS_SaveGame::LoadGame()
{
	UYetiOS_SaveGame* Local_SaveLoadInstance = Cast<UYetiOS_SaveGame>(UGameplayStatics::CreateSaveGameObject(UYetiOS_SaveGame::StaticClass()));
	const FString MySaveSlotName = Local_SaveLoadInstance->SaveSlotName;
	const uint32 MyUserIndex = Local_SaveLoadInstance->UserIndex;

	if (UGameplayStatics::DoesSaveGameExist(MySaveSlotName, MyUserIndex))
	{
		Local_SaveLoadInstance = Cast<UYetiOS_SaveGame>(UGameplayStatics::LoadGameFromSlot(MySaveSlotName, MyUserIndex));
		return Local_SaveLoadInstance;
	}

	return nullptr;
}

#undef printlog_veryverbose
