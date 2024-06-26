// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.


#include "Core/YetiOS_SaveGame.h"
#include "Core/YetiOS_Core.h"
#include "Core/YetiOS_DirectoryBase.h"
#include "Core/YetiOS_BaseProgram.h"
#include "Devices/YetiOS_DeviceManagerActor.h"
#include "Devices/YetiOS_PortableDevice.h"
#include "Hardware/YetiOS_Motherboard.h"
#include "Hardware/YetiOS_HardDisk.h"
#include "Templates/SubclassOf.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogYetiOsSaveGame, All, All)

#define printlog_error(Param1)			UE_LOG(LogYetiOsSaveGame, Error, TEXT("%s"), *FString(Param1))
#define printlog_veryverbose(Param1)	UE_LOG(LogYetiOsSaveGame, VeryVerbose, TEXT("%s"), *FString(Param1))

static const float SAVE_VERSION = 2.0;

UYetiOS_SaveGame::UYetiOS_SaveGame()
{
	SaveSlotName = "YetiTech_OperatingSystemSimulator";
	UserIndex = 0;
}

const bool UYetiOS_SaveGame::SaveGame(const class UYetiOS_BaseDevice* InDevice)
{
	if (InDevice && InDevice->GetSaveGameClass())
	{
		const AYetiOS_DeviceManagerActor* MyDeviceManager = Cast<AYetiOS_DeviceManagerActor>(InDevice->GetOuter());
		if (MyDeviceManager && MyDeviceManager->CanSaveGame())
		{
			UYetiOS_SaveGame* SaveGameInstance = Cast<UYetiOS_SaveGame>(UGameplayStatics::CreateSaveGameObject(InDevice->GetSaveGameClass()));
			SaveGameInstance->SaveVersion = SAVE_VERSION;
			SaveGameInstance->DeviceData.bSaveLoad_OsInstalled = InDevice->IsOperatingSystemInstalled();
			SaveGameInstance->DeviceData.SaveLoad_RemainingSpace = InDevice->GetMotherboard()->GetHardDisk()->GetRemainingSpace();

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

const UYetiOS_SaveGame* UYetiOS_SaveGame::LoadGame(const class UYetiOS_BaseDevice* InDevice)
{
	UYetiOS_SaveGame* Local_SaveLoadInstance = Cast<UYetiOS_SaveGame>(UGameplayStatics::CreateSaveGameObject(InDevice->GetSaveGameClass()));
	if (Local_SaveLoadInstance)
	{
		const FString MySaveSlotName = Local_SaveLoadInstance->SaveSlotName;
		const uint32 MyUserIndex = Local_SaveLoadInstance->UserIndex;
		Local_SaveLoadInstance->ConditionalBeginDestroy();
		Local_SaveLoadInstance = nullptr;

		if (UGameplayStatics::DoesSaveGameExist(MySaveSlotName, MyUserIndex))
		{
			Local_SaveLoadInstance = Cast<UYetiOS_SaveGame>(UGameplayStatics::LoadGameFromSlot(MySaveSlotName, MyUserIndex));
			if (Local_SaveLoadInstance->SaveVersion != SAVE_VERSION)
			{
				printlog_error(FString::Printf(TEXT("Failed to load save game. Version mismatch. Loaded save version: %f. Expected version: %f"), Local_SaveLoadInstance->SaveVersion, SAVE_VERSION));
				Local_SaveLoadInstance->ConditionalBeginDestroy();
				Local_SaveLoadInstance = nullptr;
			}
		}
	}
	else
	{
		printlog_error("Failed to load game. Load Game Instance was null.");
	}

	return Local_SaveLoadInstance;
}

#undef printlog_error
#undef printlog_veryverbose
