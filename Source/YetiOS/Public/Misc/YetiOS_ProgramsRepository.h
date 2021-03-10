// Copyright 2019 YetiTech Studios, Pvt Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Core/YetiOS_BaseProgram.h"
#include "YetiOS_Types.h"
#include "YetiOS_ProgramsRepository.generated.h"

/*************************************************************************
* File Information:
YetiOS_ProgramsRepository.h

* Description:
Programs repository simulates the Linux repository package system where
programs can be installed from the terminal using apt command.

It also contains installer setups that can be installed via Store.
*************************************************************************/
UCLASS(hidedropdown, Blueprintable, NotBlueprintType)
class YETIOS_API UYetiOS_ProgramsRepository : public UObject
{
	GENERATED_BODY()

private:

	/** Set of programs */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Programs Repository")
	TSet<FYetiOS_RepoProgram> Programs;

	/** Installers for the store */
	UPROPERTY(EditDefaultsOnly, Category = "Yeti OS Programs Repository")
	TSet<TSubclassOf<class UYetiOS_AppInstaller>> StoreInstallers;
	
public:
	
	FORCEINLINE TSet<FYetiOS_RepoProgram> GetProgramsFromRepository() const { return Programs; }
	FORCEINLINE TArray<TSubclassOf<class UYetiOS_AppInstaller>> GetStoreInstallers() const { return StoreInstallers.Array(); }
	FORCEINLINE bool IsInstalledWithOS(TSubclassOf<class UYetiOS_BaseProgram> InProgramToCheck) const
	{
		for (const auto& It : Programs)
		{
			if (It.ProgramClass == InProgramToCheck)
			{
				return It.bInstallWithOS;
			}
		}

		return false;
	}

	FORCEINLINE TArray<UYetiOS_BaseProgram*> GetDefaultProgramObjects(const bool bIgnoreInstalledWithOS) const
	{
		TArray<UYetiOS_BaseProgram*> RetVal;
		for (const auto& It : Programs)
		{
			if (It.bInstallWithOS && bIgnoreInstalledWithOS)
			{
				continue;
			}

			UYetiOS_BaseProgram* Local_CDO = It.ProgramClass->GetDefaultObject<UYetiOS_BaseProgram>();
			if (Local_CDO->SupportsStore())
			{
				RetVal.Add(Local_CDO);
			}
		}

		return RetVal;
	}
};
